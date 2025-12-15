// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GlobalHUDSubsystem.h"
#include "UResurrectionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Slider.h"
#include "Components/WidgetComponent.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Data/HUDData.h"
#include "Data/MusicGenericData.h"
#include "Editor/PipouCharacterSettings.h"
#include "Game/GlobalGameSubsystem.h"
#include "Logging/StructuredLog.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "UI/UMusicNote.h"
#include "UI/PartitionFinish.h"
#include "Character/PipouCharacter.h"
#include "PNJ/Bird.h"
#include "UI/BirdWidget.h"


// ---- GAME INSTANCE SUBSYSTEM ---- 
void UGlobalHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	Init();
}


void UGlobalHUDSubsystem::Tick(float DeltaTime)
{
	// Manage reset color.
	if (TimerBeforeResetingColor > 0)
	{
		TimerBeforeResetingColor -= DeltaTime;

		if (TimerBeforeResetingColor >= 0.2f)
		{
			TimerBeforeResetingColor = 0;
		}
	}

	// Manage Mistake Render.
	if (!CurrentMistakeMaterialInstance) return;

	bool MistakeAnimationShouldBeUpdated = !FMath::IsNearlyEqual(CurrentMaterialRadius, TargetMaterialRadius, 0.001f) ||
		  !FMath::IsNearlyEqual(CurrentMaterialThickness, TargetMaterialThickness, 0.001f);
	
	if (MistakeAnimationShouldBeUpdated)
	{
		CurrentMaterialThickness = FMath::FInterpTo(CurrentMaterialThickness, TargetMaterialThickness, DeltaTime, ThicknessInterpolationSpeed);
		CurrentMaterialRadius = FMath::FInterpTo(CurrentMaterialRadius, TargetMaterialRadius, DeltaTime, RadiusInterpolationSpeed);

		CurrentMistakeMaterialInstance->SetScalarParameterValue("Thickness", CurrentMaterialThickness);
		CurrentMistakeMaterialInstance->SetScalarParameterValue("Radius", CurrentMaterialRadius);
	}
}


// ---- MUSIC UI ----
void UGlobalHUDSubsystem::DisplayResurrectionWidget()
{
	if (WBPResurrectionClass == nullptr) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;

	WBPResurrectionInstance = CreateWidget<UResurrectionWidget>(PC, WBPResurrectionClass);
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->AddToViewport();

		// Manage Mistake PostProcess
		InitMistakeMaterial();
		
		UCanvasPanelSlot* PartitionSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->PartitionBox->Slot);
		if (!PartitionSlot) return;

		UCanvasPanelSlot* SliderBoxSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->SliderBox->Slot);
		if (!SliderBoxSlot) return;

		float MiddleOfPitchSliderPosX = SliderBoxSlot->GetPosition().X + (SliderBoxSlot->GetSize().X / 2);
		float BoundsXMaxPartitionBox = PartitionSlot->GetSize().X;
		UIOffset = (BoundsXMaxPartitionBox - MiddleOfPitchSliderPosX);
	}
}

void UGlobalHUDSubsystem::RemoveResurrectionWidget()
{
	if (UCameraWorldSubsystem* CameraWorldSubsystem = GetWorld()->GetSubsystem<UCameraWorldSubsystem>())
	{
		FPostProcessSettings& CameraPostProcess = CameraWorldSubsystem->CameraMain->PostProcessSettings;
		CameraPostProcess.RemoveBlendable(CurrentMistakeMaterialInstance);
		CameraWorldSubsystem->CameraMain->SetPostProcessBlendWeight(0.f);
	}

	CurrentMistakeMaterialInstance = nullptr;
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->RemoveFromParent();
		WBPResurrectionInstance = nullptr;

		// Clear the array of notes spawned during the music.
		NotesInstanciated.Empty();
	}
}

void UGlobalHUDSubsystem::DisplayPartitionFinish(const FString& TextInThePartition)
{
	if (WBPPartitionFinishClass == nullptr) return;
	
	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;

	WBPPartitionFinishInstance = CreateWidget<UPartitionFinish>(PC, WBPPartitionFinishClass);

	if (WBPPartitionFinishInstance != nullptr)
	{
		WBPPartitionFinishInstance->AddToViewport();
		WBPPartitionFinishInstance->SetWinLostText(TextInThePartition);
		WBPPartitionFinishInstance->PlayAnimationPartitionFinishStart();
	}
}

void UGlobalHUDSubsystem::RemovePartitionFinish() const
{
	if (WBPPartitionFinishInstance != nullptr)
	{
		WBPPartitionFinishInstance->PlayAnimationPartitionFinishExit();

		FTimerHandle CanDestroy;
		GetWorld()->GetTimerManager().ClearTimer(CanDestroy);

		GetWorld()->GetTimerManager().SetTimer(
			CanDestroy, [this]()
			{
				WBPPartitionFinishInstance->RemoveFromParent();
			},
			2.f,
			false
		);
	}
}

void UGlobalHUDSubsystem::SpawnNotesPartition(const ASkeletonController* CurrentSkeleton)
{
	if (WBPNoteClass == nullptr) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;

	DistancePreviousFrequencies = 0;
	
	for (F_Note Note : CurrentSkeleton->MySkeleton->Notes)
	{
		if (!WBPResurrectionInstance) return;
		
		// Create Note Widget.
		WBPNoteInstance = CreateWidget<UMusicNote>(PC, WBPNoteClass);
		if (!WBPNoteInstance) return;

		// Add the note into the parent NotesBox.
		WBPResurrectionInstance->NotesBox->AddChildToCanvas(WBPNoteInstance);
		
		// Get Note Widget Slot.
		UCanvasPanelSlot* NoteSlotInstance = Cast<UCanvasPanelSlot>(WBPNoteInstance->Slot);
		if (!NoteSlotInstance) return;

		// Get SpawnPoint depending on the Pitch of the note. 
		UUserWidget* SpawnPointFromPitch = WBPResurrectionInstance->GetSpawnPointFromInputPitch(Note.Pitch);
		if (!SpawnPointFromPitch) return;

		// Get SpawnPoint Slot.
		UCanvasPanelSlot* SpawnPointSlot = Cast<UCanvasPanelSlot>(SpawnPointFromPitch->Slot);
		if (!SpawnPointSlot) return;
		
		// Calculate Note Pos Y with SpawnPointSlot.
		float PosY = SpawnPointSlot->GetPosition().Y + 10.f;
		
		// Calculate Note Pos X.
		float PosX = (Note.Frequency * RatioDistance) + DistancePreviousFrequencies;
		
		// Set Note Pos with Note Slot.
		FVector2D NotePos = FVector2D(PosX, PosY);
		NoteSlotInstance->SetPosition(NotePos);

		// Set Music Note Type depending on the input action of the note.
		WBPNoteInstance->SetNoteTexture(GetImageTextureFromNoteInput(Note.InputAction));

		// Add the note instantiated to an array to use in the music mechanic.
		NotesInstanciated.Add(WBPNoteInstance);

		// Increment the DistancePreviousFrequencies with the new frequency of the current note spawned.
		DistancePreviousFrequencies += Note.Frequency * RatioDistance;
	}

	// Get NotesBoxSlot from NoteBox (Containers of the spawned note).
	NotesBoxSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->NotesBox->Slot);
	if (!NotesBoxSlot) return;

	UCanvasPanelSlot* TopLineSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->PitchMinusOne->Slot);
	if (!TopLineSlot) return;
	
	NotesBoxSlot->SetPosition(FVector2D(NotesBoxSlot->GetPosition().X + TopLineSlot->GetSize().X, NotesBoxSlot->GetPosition().Y));
	
	// Set the size of the notes box slot. Size is set to all the frequencies + the UiOffset size.
	NotesBoxSlot->SetSize(FVector2D(DistancePreviousFrequencies + UIOffset, NotesBoxSlot->GetSize().Y));

	// Set the start point of the lerp and the end point based on slot size and position.
	MovementStartPoint = NotesBoxSlot->GetPosition().X;
	MovementEndPoint = MovementStartPoint - NotesBoxSlot->GetSize().X - UIOffset;
}

void UGlobalHUDSubsystem::MovePartition(float DeltaTime)
{
	if (!WBPNoteInstance) return;

	// All our frequencies in the ratio distance.
	float PreviousFrequenciesInTime = DistancePreviousFrequencies / RatioDistance;
	// float PreviousFrequenciesInTime = (DistancePreviousFrequencies / RatioDistance);

	// The UiOffset in the ratio distance.
	float UiOffsetInTime = UIOffset / RatioDistance;
	// float UiOffsetInTime = (UIOffset / RatioDistance);

	UCanvasPanelSlot* SliderBoxSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->SliderBox->Slot);
	if (!SliderBoxSlot) return;

	// UiSpeed = d / t
	UISpeed = ((DistancePreviousFrequencies + UIOffset) + (SliderBoxSlot->GetSize().X / 2)) / ((PreviousFrequenciesInTime + UiOffsetInTime));
	// UISpeed = (DistancePreviousFrequencies + UIOffset) / (PreviousFrequenciesInTime + UiOffsetInTime);
	
	if (NotesBoxSlot->GetPosition().X >= MovementEndPoint)
	{
		float NewNoteBoxPosX = UISpeed * DeltaTime;
		NotesBoxSlot->SetPosition(FVector2D(NotesBoxSlot->GetPosition().X - NewNoteBoxPosX, NotesBoxSlot->GetPosition().Y));
	}
}

float UGlobalHUDSubsystem::GetUISpeed() const
{
	return UISpeed;
}

float UGlobalHUDSubsystem::GetUIOffset() const
{
	return UIOffset;
}


// ---- UTILITIES ----
void UGlobalHUDSubsystem::Init()
{
	// Init Global Game Subsystem.
	GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
	// Init Myself in Global Game Subsystem
	if (GlobalGameSubsystem) GlobalGameSubsystem->SetHUD(this);

	// Init Subsystem Settings.
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;

	UMusicGenericData* MusicGenericData = SubsystemSettings->MusicGenericData.LoadSynchronous();
	if (!MusicGenericData) return;

	// Init PipouCharacterSettings.
	const UPipouCharacterSettings* CharacterSettings = GetDefault<UPipouCharacterSettings>();
	if (!CharacterSettings) return;

	// Load InputData.
	InputData = CharacterSettings->InputData.LoadSynchronous();
	if (!InputData) UE_LOG(LogTemp, Fatal, TEXT("UGlobalHUDSubsystem::InputData is NULL"));

	// Initialize from SubsystemSettings Resurrection Widget class, Note Class and PartitionFinishClass.
	WBPResurrectionClass = SubsystemSettings->WBPResurrectionClass;
	WBPNoteClass = SubsystemSettings->WBPNoteClass;
	WBPPartitionFinishClass = SubsystemSettings->WBPPartitionFinishClass;
	WBPPauseMenuClass = SubsystemSettings->WBPPauseMenuClass;

	// Init HUD Data
	HUDData = SubsystemSettings->HUDData.LoadSynchronous();

	// init image from input
	TextureFromNoteInput=
	{
		{ InputData->InputNoteY, HUDData->NoteUp },
		{ InputData->InputNoteB, HUDData->NoteRight },
		{ InputData->InputNoteA, HUDData->NoteDown },
		{ InputData->InputNoteX, HUDData->NoteLeft },
	};

	BirdTextureFromNoteInput =
	{
		{ InputData->InputNoteY, HUDData->NoteUpInteraction },
		{ InputData->InputNoteB, HUDData->NoteRightInteraction },
		{ InputData->InputNoteA, HUDData->NoteDownInteraction },
		{ InputData->InputNoteX, HUDData->NoteLeftInteraction },
	};

	// Init Material Instance
	MistakeBaseMaterial = HUDData->MistakeMaterialInstance;
}

UTexture2D* UGlobalHUDSubsystem::GetImageTextureFromNoteInput(const UInputAction* NoteInput) const
{
	return TextureFromNoteInput[NoteInput];
}

UTexture2D* UGlobalHUDSubsystem::GetImageBirdTextureFromNoteInput(const UInputAction* NoteInput) const
{
	return BirdTextureFromNoteInput[NoteInput];
}

void UGlobalHUDSubsystem::SetMusicWorldSubsystem(UMusicWorldSubsystem* NewMusicSubsystem)
{
	MusicWorldSubsystem = NewMusicSubsystem;
}


// ---- FEEDBACK COLORS NOTES ---- 
void UGlobalHUDSubsystem::SetImageColor(UImage* CurrentImage, FLinearColor NewColor)
{
	Internal_SetImageColor(CurrentImage, NewColor);
	
	FTimerHandle Handle;
	GetWorld()->GetTimerManager().ClearTimer(Handle);
	GetWorld()->GetTimerManager().SetTimer(Handle,[this, CurrentImage]()
		{
			Internal_SetImageColor(CurrentImage, FLinearColor::White);
		},
		0.2f,
		false
	);
}

void UGlobalHUDSubsystem::Internal_SetImageColor(UImage* CurrentImage, FLinearColor NewColor)
{
	if (!CurrentImage)
	{
		UE_LOGFMT(LogTemp, Warning, "Color can't be changed because CurrentObject is nullptr !");
		return;
	}
	
	if (UImage* Image = Cast<UImage>(CurrentImage))
	{
		FSlateBrush ImageBrush = Image->GetBrush();
		ImageBrush.TintColor = FSlateColor(NewColor);
		Image->SetBrush(ImageBrush);
	}
}


// ---- UI WORLD ----
void UGlobalHUDSubsystem::SetWidgetVisibility(UUserWidget* Widget, bool Visibility)
{
	if (Visibility)
		Widget->SetVisibility(ESlateVisibility::Visible);
	else
		Widget->SetVisibility(ESlateVisibility::Hidden);
}

void UGlobalHUDSubsystem::ValideWidget()
{
	BirdWidget->ToucheV();
}

void UGlobalHUDSubsystem::FolseWidget()
{
	BirdWidget->ToucheF();
}

void UGlobalHUDSubsystem::RemoveBirdWidget()
{
	BirdWidget->RemoveWidget();
}


// ---- MISTAKE POST PROCESS ---- 
void UGlobalHUDSubsystem::InitMistakeMaterial()
{
	if (!MistakeBaseMaterial) return;
	
	if (UCameraWorldSubsystem* CameraWorldSubsystem = GetWorld()->GetSubsystem<UCameraWorldSubsystem>())
	{
		FPostProcessSettings& CameraPostProcess = CameraWorldSubsystem->CameraMain->PostProcessSettings;
		CurrentMistakeMaterialInstance = UMaterialInstanceDynamic::Create(MistakeBaseMaterial, this);

		if (!CurrentMistakeMaterialInstance) return;

		CameraPostProcess.AddBlendable(CurrentMistakeMaterialInstance, 1.f);
		CameraWorldSubsystem->CameraMain->SetPostProcessBlendWeight(1.f);
		
		ResetMistakeEffect();
	}
}

void UGlobalHUDSubsystem::ApplyMistakeIncrease(int CurrentFail, int MaxFail)
{
	if (!CurrentMistakeMaterialInstance) return;

	float FailRatio = 1.f - (CurrentFail / MaxFail); 

	TargetMaterialRadius = FMath::Lerp(MinRadius, MaxRadius, FailRatio);
	TargetMaterialThickness = FMath::Lerp(MinThickness, MaxThickness, FailRatio);
}

void UGlobalHUDSubsystem::ResetMistakeEffect()
{
	TargetMaterialRadius = MaxRadius;
	TargetMaterialThickness = MinThickness;
}

void UGlobalHUDSubsystem::ForceMistakeCollapse()
{
	TargetMaterialRadius = MinRadius;
	TargetMaterialThickness = MaxThickness;

	FTimerHandle WaitBeforeGoingBackToUnCollapse;
	GetWorld()->GetTimerManager().ClearTimer(WaitBeforeGoingBackToUnCollapse);

	GetWorld()->GetTimerManager().SetTimer(
		WaitBeforeGoingBackToUnCollapse, [this]()
		{
			SetMistakeToZero();
		},
		1.f,
		false
		);
}

void UGlobalHUDSubsystem::SetMistakeToZero()
{
	TargetMaterialRadius = 3.f;
	TargetMaterialThickness = 0.f;
}


void UGlobalHUDSubsystem::InitBirdWidget(ABird* Bird)
{
	BirdWidget = Cast<UBirdWidget>(Bird->FindComponentByClass<UWidgetComponent>()->GetWidget());
}

void UGlobalHUDSubsystem::ResetBirdWidget()
{
	for (auto Image : BirdWidget->Images)
	{
		Image->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UGlobalHUDSubsystem::DisplayNotesForSkeletonInteraction(const UInputAction* InputAction)
{
	if (!GlobalGameSubsystem || !BirdWidget) return;

	// Récupérer l’index en le clampant
	const int32 MaxIndex = BirdWidget->Images.Num() - 1;
	int32 Index = FMath::Clamp(GlobalGameSubsystem->InputPressed.Num() - 1, 0, MaxIndex);

	// Récupérer l'image
	if (UTexture2D* Text = GetImageBirdTextureFromNoteInput(InputAction))
	{
		BirdWidget->Images[Index]->SetBrushFromTexture(Text);
	}

	// Couleur (pleine opacité)
	BirdWidget->Images[Index]->SetColorAndOpacity(FLinearColor::White);
	

	// Afficher l'image
	// BirdWidget->Images[Index]->SetVisibility(ESlateVisibility::Visible);
	
	
}

// ---- UTILITIES ----
void UGlobalHUDSubsystem::DisplayPauseMenu()
{
	if (!WBPPauseMenuClass) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;
	
	WBPPauseMenuInstance = CreateWidget<UUserWidget>(PC, WBPPauseMenuClass);
	if (!WBPPauseMenuInstance) return;

	WBPPauseMenuInstance->AddToViewport();
}

void UGlobalHUDSubsystem::RemovePauseMenu()
{
	if (WBPPauseMenuInstance != nullptr)
	{
		WBPPauseMenuInstance->RemoveFromParent();
		WBPPauseMenuInstance = nullptr;
	}
}



