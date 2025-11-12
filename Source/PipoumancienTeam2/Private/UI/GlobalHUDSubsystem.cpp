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
#include "Settings/SubsystemSettings.h"
#include "UI/USlot.h"

void UGlobalHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Init();
}

void UGlobalHUDSubsystem::Init()
{
	//Init Global Game Subsystem
	GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();

	// Get Subsystem Settings and PipouCharacterSettings
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;

	const UPipouCharacterSettings* CharacterSettings = GetDefault<UPipouCharacterSettings>();
	if (!CharacterSettings) return;

	InputData = CharacterSettings->InputData;
	WBPResurrectionClass = SubsystemSettings->WBPResurrectionClass;
	WBPNoteClass = SubsystemSettings->WBPNoteClass;
	
	MusicNoteFromInputAction =
	{
		{ InputData->InputNoteA, EMusicNoteType::A },
		{ InputData->InputNoteB, EMusicNoteType::B },
		{ InputData->InputNoteY, EMusicNoteType::Y },
		{ InputData->InputNoteX, EMusicNoteType::X }
	};
}

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

void UGlobalHUDSubsystem::SpawnNotesPartition(F_Skeleton* CurrentSkeleton)
{
	if (WBPNoteClass == nullptr) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;

	DistancePreviousFrequencies = 0;
	
	for (F_Note Note : CurrentSkeleton->Notes)
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
	
	UMusicWorldSubsystem* MusicWorldSubsystem = GetWorld()->GetSubsystem<UMusicWorldSubsystem>();
	if (!MusicWorldSubsystem) return;

	float PreviousFrequencies = DistancePreviousFrequencies / RatioDistance;
	float UISpeed = (DistancePreviousFrequencies / (PreviousFrequencies * MusicWorldSubsystem->Speed));
	
	if (NotesBoxSlot->GetPosition().X >= EndPointLerp)
	{
		PosXDeux -= UISpeed * DeltaTime;
		NotesBoxSlot->SetPosition(FVector2D(PosXDeux, NotesBoxSlot->GetPosition().Y));
	}
}

// Utilities functions
EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}

