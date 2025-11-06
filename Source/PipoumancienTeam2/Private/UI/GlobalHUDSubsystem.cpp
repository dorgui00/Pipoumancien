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
#include "Kismet/GameplayStatics.h"
#include "Settings/SubsystemSettings.h"
#include "UI/USlot.h"


class UPipouCharacterSettings;

void UGlobalHUDSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Init();
}

void UGlobalHUDSubsystem::Init()
{
	//Init Global Game Subsystem
	GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();

	// Init fields via Settings
	const USubsystemSettings* Settings = GetDefault<USubsystemSettings>();

	//InputData = Settings->InputData; // TO EDIT
	WBPResurrectionClass = Settings->WBPResurrectionClass;
	WBPNoteClass = Settings->WBPNoteClass;
	
	InputData = LoadObject<UPipouCharacterInputData>(nullptr, TEXT("/Game/Pipoumancien/Inputs/DA_Character_Inputs.DA_Character_Inputs"));
	// WBPResurrectionClass = LoadClass<UResurrectionWidget>(nullptr, TEXT("/Game/Pipoumancien/Blueprint/UI/WBP_UI_Reanimation.WBP_UI_Reanimation"));
	// WBPNoteClass = LoadClass<USlot>(nullptr, TEXT("/Game/Pipoumancien/Blueprint/UI/WBP_UI_Reanimation.WBP_UI_Reanimation"));
	
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
	// if (WBPResurrectionClass == nullptr) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;
	
	WBPResurrectionInstance = CreateWidget<UResurrectionWidget>(PC, UResurrectionWidget::StaticClass());
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->AddToViewport();
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
	// if (WBPNoteClass == nullptr) return ;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;

	float DistancePreviousFrequencies = 0;
	
	for (F_Note Note : CurrentSkeleton->Notes)
	{
		if (!WBPResurrectionInstance) return;
		
		// Create Slot
		WBPNoteInstance = CreateWidget<USlot>(PC, USlot::StaticClass());
		if (!WBPNoteInstance) return;

		// Add the note to the parent spawn notes
		WBPResurrectionInstance->SpawnNotes->AddChildToCanvas(WBPNoteInstance);
		
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
		float PosX = (Note.Frequency / RatioDistance) + DistancePreviousFrequencies;
		
		// Set Slot Pos
		FVector2D NotePos = FVector2D(PosX, PosY);
		NoteSlotInstance->SetPosition(NotePos);
		
		// Set Music Note Type
		WBPNoteInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(Note.InputAction));
		
		DistancePreviousFrequencies += Note.Frequency / RatioDistance;
	}
}

// Utilities functions
EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}

