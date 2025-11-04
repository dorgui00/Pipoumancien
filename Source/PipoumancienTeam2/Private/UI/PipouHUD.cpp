// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PipouHUD.h"

#include "InputTriggers.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.h"
#include "Character/PipouCharacterInputData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/PanelSlot.h"
#include "UI/USlot.h"

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

UCanvasPanelSlot* APipouHUD::AddWbpSlotInstance(APlayerController* PlayerController, float InputPitch, UInputAction* InputAction)
{
	if (WBPSlotClass == nullptr) return nullptr;

	WBPSlotInstance = CreateWidget<USlot>(PlayerController, WBPSlotClass);

	if (WBPSlotInstance != nullptr)
	{
		WBPSlotInstance->AddToViewport();
		
		// Get WBP canvas note panel slot 
		UCanvasPanelSlot* SlotInstancePanel = Cast<UCanvasPanelSlot>(WBPSlotInstance->Slot);
		if (SlotInstancePanel == nullptr) return nullptr;

		// Get SpawnPoint from ResurrectionWidget, from the input pitch of the input action of the notes
		if (WBPResurrectionInstance == nullptr) return nullptr;
		UUserWidget* SpawnPoint = WBPResurrectionInstance->GetSpawnPointFromInputPitch(InputPitch);

		// Get slot canvas panel from spawnpoint
		if (SpawnPoint == nullptr) return nullptr;
		UCanvasPanelSlot* SpawnPointPanel = Cast<UCanvasPanelSlot>(SpawnPoint->Slot);

		// Set spawn position of the slot to the spawnpoint position 
		SlotInstancePanel->SetPosition(SpawnPointPanel->GetPosition());

		// Set Type of note the slot is
		WBPSlotInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(InputAction));

		// Return the slot canvas panel 
		return SlotInstancePanel;
	}

	return nullptr;
}

// Utilities functions
EMusicNoteType APipouHUD::GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const
{
	return MusicNoteFromInputAction[InputAction];
}


