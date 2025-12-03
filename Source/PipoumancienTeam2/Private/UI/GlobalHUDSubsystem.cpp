// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GlobalHUDSubsystem.h"

#include "UResurrectionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Slider.h"
#include "Components/WidgetComponent.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Data/HUDData.h"
#include "Editor/PipouCharacterSettings.h"
#include "Game/GlobalGameSubsystem.h"
#include "Logging/StructuredLog.h"
#include "Music/MusicWorldSubsystem.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "UI/UMusicNote.h"
#include "UI/PartitionFinish.h"
#include "UI/SkeletonInteractionWidget.h"


// ---- GAME INSTANCE SUBSYSTEM ---- 
void UGlobalHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Init();
}

void UGlobalHUDSubsystem::Tick(float DeltaTime)
{
	if (TimerBeforeResetingColor > 0)
	{
		TimerBeforeResetingColor -= DeltaTime;

		if (TimerBeforeResetingColor >= 0.2f)
		{
			
			TimerBeforeResetingColor = 0;
		}
	}
}

// ---- WORLD UI ---
void UGlobalHUDSubsystem::SpawnSkeletonInteractionWidget()
{
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	UWidgetComponent* WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(Root); // middle of player

	WidgetComponent->SetWidgetClass(USkeletonInteractionWidget::StaticClass());
		
	// if (USkeletonInteractionWidget* SkeletonInteractionWidget = Cast<USkeletonInteractionWidget>(WidgetComponent->GetUserWidgetObject()))
	// {
	// 	
	// }
	
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

		UCanvasPanelSlot* PartitionSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->PartitionBox->Slot);
		if (!PartitionSlot) return;

		UCanvasPanelSlot* SliderBoxSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->SliderBox->Slot);
		if (!SliderBoxSlot) return;

		float MiddleOfPitchSliderPosX = SliderBoxSlot->GetPosition().X + (SliderBoxSlot->GetSize().X / 2);
		float BoundsXMaxPartitionBox = PartitionSlot->GetSize().X;
		// // How many of my slider I can put in the partition : 110 * 9,35px (result of PartitionSlot->GetSize().X / SliderSlot->GetSize().X).
		// float PartitionInSliderRatio = PartitionSlot->GetSize().X / SliderBoxSlot->GetSize().X;
		// // How many 9,35 are in my slider (110px) = 11,8. I can put 11,8 of 9,35 in my slider.
		// float EffectiveSliderPartSize = (SliderBoxSlot->GetSize().X / MiddleOfPitchSliderPosX);
		// // UiOffset is all the partition size minus the size of the slider for it to time for the qte when the note is the middle of the circle of the slider.
		// UIOffset = PartitionInSliderRatio * (SliderBoxSlot->GetSize().X - EffectiveSliderPartSize);
		UIOffset = (BoundsXMaxPartitionBox - MiddleOfPitchSliderPosX);
	}
}

void UGlobalHUDSubsystem::RemoveResurrectionWidget()
{
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
		float PosY = SpawnPointSlot->GetPosition().Y;
		
		// Calculate Note Pos X.
		float PosX = (Note.Frequency * RatioDistance) + DistancePreviousFrequencies;
		
		// Set Note Pos with Note Slot.
		FVector2D NotePos = FVector2D(PosX, PosY);
		NoteSlotInstance->SetPosition(NotePos);

		// Set Music Note Type depending on the input action of the note.
		WBPNoteInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(Note.InputAction));

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

	// Init Subsystem Settings.
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;

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

	// Initialize the association of InputAction to MusicNoteType.
	MusicNoteFromInputAction =
	{
		{ InputData->InputNoteA, EMusicNoteType::A },
		{ InputData->InputNoteB, EMusicNoteType::B },
		{ InputData->InputNoteY, EMusicNoteType::Y },
		{ InputData->InputNoteX, EMusicNoteType::X }
	};

	// Init HUD Data
	HUDData = SubsystemSettings->HUDData.LoadSynchronous();

	// init image from input
	ImageFromNoteInput = {
		{InputData->InputNoteY, HUDData->NoteUp },
		{InputData->InputNoteB, HUDData->NoteRight },
		{InputData->InputNoteA, HUDData->NoteDown },
		{InputData->InputNoteX, HUDData->NoteLeft },
	};
}

EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
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

void UGlobalHUDSubsystem::DisplayNotesForSkeletonInteraction(UInputAction* InputAction)
{
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

