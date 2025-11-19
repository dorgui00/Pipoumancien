// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GlobalHUDSubsystem.h"

#include "UResurrectionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Slider.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Editor/PipouCharacterSettings.h"
#include "Game/GlobalGameSubsystem.h"
#include "Music/MusicWorldSubsystem.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "UI/UMusicNote.h"

#pragma region GameInstanceSubsystem
void UGlobalHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Init();
}

void UGlobalHUDSubsystem::Tick(float DeltaTime)
{
	if (TimerForResetingColor > 0)
	{
		TimerForResetingColor -= DeltaTime;

		if (TimerForResetingColor >= 0.2f)
		{
			
			TimerForResetingColor = 0;
		}
	}
}

#pragma endregion 

#pragma region Music
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
		
		// How many of my slider I can put in the partition : 110 * 9,35px (result of PartitionSlot->GetSize().X / SliderSlot->GetSize().X).
		float PartitionInSliderRatio = PartitionSlot->GetSize().X / SliderBoxSlot->GetSize().X;
		// How many 9,35 are in my slider (110px) = 11,8. I can put 11,8 of 9,35 in my slider.
		float EffectiveSliderPartSize = (SliderBoxSlot->GetSize().X / PartitionInSliderRatio) / 2;
		// UiOffset is all the partition size minus the size of the slider for it to time for the qte when the note is the middle of the circle of the slider.
		UiOffset = PartitionInSliderRatio * (SliderBoxSlot->GetSize().X - EffectiveSliderPartSize);
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

	// Set the size of the notes box slot. Size is set to all the frequencies + the UiOffset size.
	NotesBoxSlot->SetSize(FVector2D(DistancePreviousFrequencies + UiOffset, NotesBoxSlot->GetSize().Y));

	// Set the start point of the lerp and the end point based on slot size and position.
	MovementStartPoint = NotesBoxSlot->GetPosition().X;
	MovementEndPoint = MovementStartPoint - NotesBoxSlot->GetSize().X;
}

void UGlobalHUDSubsystem::MovePartition(float DeltaTime)
{
	if (!WBPNoteInstance) return;

	// All our frequencies in the ratio distance. 
	float PreviousFrequenciesInRatioDist = DistancePreviousFrequencies / RatioDistance;

	// The UiOffset in the ratio distance.
	float UiOffsetInRatioDist = UiOffset / RatioDistance;

	// UiSpeed = d / t
	UISpeed = (DistancePreviousFrequencies + UiOffset) / ((PreviousFrequenciesInRatioDist + UiOffsetInRatioDist) * MusicWorldSubsystem->MusicGlobalSpeed);
	
	if (NotesBoxSlot->GetPosition().X >= MovementEndPoint)
	{
		float NewNoteBoxPosX = UISpeed * DeltaTime;
		NotesBoxSlot->SetPosition(FVector2D(NotesBoxSlot->GetPosition().X - NewNoteBoxPosX, NotesBoxSlot->GetPosition().Y));
	}
}

void UGlobalHUDSubsystem::RewindPartition(int CurrentNoteIndex, const F_Note& NewNote)
{
	// Current Note Widget.
	UMusicNote* CurrentWidgetNote = NotesInstanciated[CurrentNoteIndex];
	if (!CurrentWidgetNote) return;

	UCanvasPanelSlot* CurrentWidgetNoteSlot = Cast<UCanvasPanelSlot>(CurrentWidgetNote->Slot);
	if (!CurrentWidgetNoteSlot) return;

	float PreviewDistance = UiOffset / 2;
	PreviewTime = PreviewDistance / UISpeed;

	float NewPartitionPosX = MovementStartPoint - CurrentWidgetNoteSlot->GetPosition().X - UiOffset + PreviewDistance;
	NotesBoxSlot->SetPosition(FVector2D(NewPartitionPosX, NotesBoxSlot->GetPosition().Y));
}

#pragma endregion

#pragma region Utilities
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

	// Initialize from SubsystemSettings Resurrection Widget class and Note Class.
	WBPResurrectionClass = SubsystemSettings->WBPResurrectionClass;
	WBPNoteClass = SubsystemSettings->WBPNoteClass;

	// Initialize the association of InputAction to MusicNoteType.
	MusicNoteFromInputAction =
	{
		{ InputData->InputNoteA, EMusicNoteType::A },
		{ InputData->InputNoteB, EMusicNoteType::B },
		{ InputData->InputNoteY, EMusicNoteType::Y },
		{ InputData->InputNoteX, EMusicNoteType::X }
	};

	// Initialize RatioDistance from Subsystem Settings.
	RatioDistance = SubsystemSettings->RatioDistance;
}

EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}

#pragma endregion
