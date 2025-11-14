// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GlobalHUDSubsystem.h"
#include "UResurrectionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Editor/PipouCharacterSettings.h"
#include "Game/GlobalGameSubsystem.h"
#include "Music/MusicWorldSubsystem.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "UI/USlot.h"

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

		if (TimerForResetingColor > 0.5f)
		{
			TimerForResetingColor = 0;
		}
	}
}

void UGlobalHUDSubsystem::Init()
{
	// Init Global Game Subsystem
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

	// Initialize from SubsystemSettings Resurrection Widget class and Note Slot Class.
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
}

// Display Resurrection Widget.
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

		UiOffset = PartitionSlot->GetSize().X;
	}
}

void UGlobalHUDSubsystem::RemoveResurrectionWidget()
{
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->RemoveFromParent();
		WBPResurrectionInstance = nullptr;
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
		
		// Create Slot
		WBPNoteInstance = CreateWidget<USlot>(PC, WBPNoteClass);
		if (!WBPNoteInstance) return;

		// Add the note to the parent spawn notes
		WBPResurrectionInstance->NotesBox->AddChildToCanvas(WBPNoteInstance);
		
		// Get Note Slot
		UCanvasPanelSlot* NoteSlotInstance = Cast<UCanvasPanelSlot>(WBPNoteInstance->Slot);
		if (!NoteSlotInstance) return;

		// Get SpawnPoint
		UUserWidget* SpawnPointFromPitch = WBPResurrectionInstance->GetSpawnPointFromInputPitch(Note.Pitch);
		if (!SpawnPointFromPitch) return;

		// Get SpawnPoint Slot
		UCanvasPanelSlot* SpawnPointSlot = Cast<UCanvasPanelSlot>(SpawnPointFromPitch->Slot);
		if (!SpawnPointSlot) return;
		
		// Calculate Note Slot Pos Y
		float PosY = SpawnPointSlot->GetPosition().Y;
		
		// Calculate Note Slot Pos X
		float PosX = (Note.Frequency * RatioDistance) + DistancePreviousFrequencies;
		
		// Set Slot Pos
		FVector2D NotePos = FVector2D(PosX, PosY);
		NoteSlotInstance->SetPosition(NotePos);
		
		// Set Music Note Type
		WBPNoteInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(Note.InputAction));

		NotesInstanciated.Add(WBPNoteInstance);
		
		DistancePreviousFrequencies += Note.Frequency * RatioDistance;
	}

	// Get NotesBoxSlot 
	NotesBoxSlot = Cast<UCanvasPanelSlot>(WBPResurrectionInstance->NotesBox->Slot);
	if (!NotesBoxSlot) return;

	// Set the size of the notes box slot
	NotesBoxSlot->SetSize(FVector2D(DistancePreviousFrequencies + UiOffset, NotesBoxSlot->GetSize().Y));

	// Set the start point of the lerp and the end point based on slot size and position
	StartPointLerp = NotesBoxSlot->GetPosition().X;
	EndPointLerp = StartPointLerp - NotesBoxSlot->GetSize().X;
}

void UGlobalHUDSubsystem::MovePartition(float DeltaTime)
{
	if (!WBPNoteInstance) return;

	float PreviousFrequencies = DistancePreviousFrequencies / RatioDistance;
	UiOffsetInTime = UiOffset / RatioDistance;
	UISpeed = (DistancePreviousFrequencies + UiOffset) / ((PreviousFrequencies + UiOffsetInTime) * MusicWorldSubsystem->Speed);
	
	if (NotesBoxSlot->GetPosition().X >= EndPointLerp)
	{
		PosXDeux = UISpeed * DeltaTime;
		NotesBoxSlot->SetPosition(FVector2D(NotesBoxSlot->GetPosition().X - PosXDeux, NotesBoxSlot->GetPosition().Y));
	}
}

// Change Partition when losing
void UGlobalHUDSubsystem::RewindPartition(int NoteIndex)
{
	if (NotesInstanciated.IsEmpty()) return;

	USlot* Note = NotesInstanciated[NoteIndex];
	if (!Note) return;

	UCanvasPanelSlot* NoteSlot = Cast<UCanvasPanelSlot>(Note->Slot);
	if (!NoteSlot) return;

	float TargetPosX = NoteSlot->GetPosition().X;

	float NewParitionPosX = (StartPointLerp - TargetPosX) - UiOffset;

	NotesBoxSlot->SetPosition(FVector2D(NewParitionPosX, NotesBoxSlot->GetPosition().Y));
}

// Utilities functions
EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}
