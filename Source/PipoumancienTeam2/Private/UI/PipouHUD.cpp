// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PipouHUD.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelSlot.h"
#include "UI/USlot.h"

void APipouHUD::BeginPlay()
{
	Super::BeginPlay();

	MusicNoteFromInputAction =
	{
		{ InputData->InputNoteA, EMusicNoteType::A },
		{ InputData->InputNoteB, EMusicNoteType::B },
		{ InputData->InputNoteY, EMusicNoteType::Y },
		{ InputData->InputNoteX, EMusicNoteType::X }
	};
}

void APipouHUD::AddWBPResurrection(APlayerController* PlayerController)
{
	if (WBPResurrectionClass == nullptr) return;
	
	WBPResurrectionInstance = CreateWidget<UResurrectionWidget>(PlayerController, WBPResurrectionClass);
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->AddToViewport();
	}
}

void APipouHUD::RemoveResurrection()
{
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->RemoveFromParent();
		WBPResurrectionInstance = nullptr;
	}
}

USlot* APipouHUD::AddWbpSlotInstance(APlayerController* PlayerController, float InputPitch, UInputAction* InputAction)
{
	if (WBPSlotClass == nullptr) return nullptr;

	WBPSlotInstance = CreateWidget<USlot>(PlayerController, WBPSlotClass);

	if (WBPSlotInstance != nullptr)
	{
		if (WBPResurrectionInstance == nullptr) return nullptr;
		WBPResurrectionInstance->SlotSpawnPoints->AddChildToCanvas(WBPSlotInstance);
		
		// Get WBP canvas note panel slot 
		UCanvasPanelSlot* SlotInstancePanel = Cast<UCanvasPanelSlot>(WBPSlotInstance->Slot);
		if (SlotInstancePanel == nullptr) return nullptr;

		// Get SpawnPoint from ResurrectionWidget, from the input pitch of the input action of the notes
		UUserWidget* SpawnPoint = WBPResurrectionInstance->GetSpawnPointFromInputPitch(InputPitch);
		UUserWidget* EndPoint = WBPResurrectionInstance->GetEndPointFromInputPitch(InputPitch);

		// Get slot canvas panel from spawnpoint
		if (SpawnPoint == nullptr) return nullptr;
		UCanvasPanelSlot* SpawnPointPanel = Cast<UCanvasPanelSlot>(SpawnPoint->Slot);

		if (EndPoint == nullptr) return nullptr;
		UCanvasPanelSlot* EndPointPanel = Cast<UCanvasPanelSlot>(EndPoint->Slot);

		WBPSlotInstance->SpawnPoint = SpawnPointPanel;
		WBPSlotInstance->EndPoint = EndPointPanel;
		
		// Set spawn position of the slot to the spawnpoint position 
		SlotInstancePanel->SetPosition(SpawnPointPanel->GetPosition());

		// Set Type of note the slot is
		WBPSlotInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(InputAction));

		// Return the slot canvas panel 
		return WBPSlotInstance;
	}

	return nullptr;
}

// Utilities functions
EMusicNoteType APipouHUD::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}


