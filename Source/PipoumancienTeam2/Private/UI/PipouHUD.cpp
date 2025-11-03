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

void APipouHUD::AddWbpSlotInstance(APlayerController* PlayerController, float InputPitch, UInputAction* InputAction)
{
	if (WBPSlotClass == nullptr) return;

	WBPSlotInstance = CreateWidget<USlot>(PlayerController, WBPSlotClass);

	if (WBPSlotInstance != nullptr)
	{
		WBPSlotInstance->AddToViewport();
		
		// WBP canvas panel slot 
		UCanvasPanelSlot* SlotInstancePanel = Cast<UCanvasPanelSlot>(WBPSlotInstance->Slot);
		if (SlotInstancePanel == nullptr) return;

		// Get SpawnPoint Height from InputPitch
		if (WBPResurrectionInstance == nullptr) return;
		UUserWidget* SpawnPoint = WBPResurrectionInstance->GetSpawnPointFromInputPitch(InputPitch);

		// Get slot canvas panel from spawnpoint
		if (SpawnPoint == nullptr) return;
		UCanvasPanelSlot* SpawnPointPanel = Cast<UCanvasPanelSlot>(SpawnPoint->Slot);

		// SetPosition of the slot 
		SlotInstancePanel->SetPosition(SpawnPointPanel->GetPosition());

		// Set Type
		WBPSlotInstance->SetSlotNote(GetMusicNoteTypeFromInputAction(InputAction));
	}
}

// Utilities functions
EMusicNoteType APipouHUD::GetMusicNoteTypeFromInputAction(UInputAction* InputAction)
{
	switch (InputAction)
	{
	case (InputAction == InputData->InputNoteA):
		return EMusicNoteType::A;
	case (InputAction == InputData->InputNoteB):
		return EMusicNoteType::B;
	case (InputAction == InputData->InputNoteY):
		return EMusicNoteType::Y;
	case (InputAction == InputData->InputNoteX):
		return EMusicNoteType::X;
	default:
		return EMusicNoteType::None;
	}
	return EMusicNoteType::None;
}


