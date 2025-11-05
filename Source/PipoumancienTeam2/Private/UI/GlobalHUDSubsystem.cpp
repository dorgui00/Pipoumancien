// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GlobalHUDSubsystem.h"
#include "UResurrectionWidget.h"
#include "Blueprint/UserWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
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

	MusicNoteFromInputAction =
	{
		{ InputData->InputNoteA, EMusicNoteType::A },
		{ InputData->InputNoteB, EMusicNoteType::B },
		{ InputData->InputNoteY, EMusicNoteType::Y },
		{ InputData->InputNoteX, EMusicNoteType::X }
	};
}

void UGlobalHUDSubsystem::AddWBPResurrection()
{
	if (WBPResurrectionClass == nullptr) return;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;
	
	WBPResurrectionInstance = CreateWidget<UResurrectionWidget>(PC, WBPResurrectionClass);
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->AddToViewport();
	}
}


void UGlobalHUDSubsystem::AddWbpSlotInstance(float InputPitch, UInputAction* InputAction)
{
	if (WBPSlotClass == nullptr) return ;

	APlayerController* PC = Cast<APlayerController>(GlobalGameSubsystem->PipouCharacters[0]->GetController());
	if (!PC) return;
	
	WBPSlotInstance = CreateWidget<USlot>(PC, WBPSlotClass);

	if (WBPSlotInstance != nullptr)
	{
		if (WBPResurrectionInstance == nullptr) 
		WBPResurrectionInstance->SlotSpawnPoints->AddChildToCanvas(WBPSlotInstance);
		
		// Get WBP canvas note panel slot 
		UCanvasPanelSlot* SlotInstancePanel = Cast<UCanvasPanelSlot>(WBPSlotInstance->Slot);
		if (SlotInstancePanel ==nullptr) return;

		// Get SpawnPoint from ResurrectionWidget, from the input pitch of the input action of the notes
		UUserWidget* SpawnPoint = WBPResurrectionInstance->GetSpawnPointFromInputPitch(InputPitch);
		UUserWidget* EndPoint = WBPResurrectionInstance->GetEndPointFromInputPitch(InputPitch);

		// Get slot canvas panel from spawnpoint
		if (SpawnPoint == nullptr) return ;
		UCanvasPanelSlot* SpawnPointPanel = Cast<UCanvasPanelSlot>(SpawnPoint->Slot);

		if (EndPoint == nullptr) return ;
		UCanvasPanelSlot* EndPointPanel = Cast<UCanvasPanelSlot>(EndPoint->Slot);

		WBPSlotInstance->SpawnPoint = SpawnPointPanel;
		WBPSlotInstance->EndPoint = EndPointPanel;
		
		// Set spawn position of the slot to the spawnpoint position 
		SlotInstancePanel->SetPosition(SpawnPointPanel->GetPosition());

		// Set Type of note the slot is
		WBPSlotInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(InputAction));
	}
}


// Utilities functions
EMusicNoteType UGlobalHUDSubsystem::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}


// 
void UGlobalHUDSubsystem::DisplayResurrection()
{
	AddWBPResurrection();
	
	for (F_Note Note : GlobalGameSubsystem->GetCurrentSkeleton()->Notes)
	{
		// Store the note canvas panel in the GameManager to make it move in the tick 
		AddWbpSlotInstance(Note.Pitch, Note.InputAction);
	}
}


void UGlobalHUDSubsystem::RemoveResurrection()
{
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->RemoveFromParent();
		WBPResurrectionInstance = nullptr;
	}
}