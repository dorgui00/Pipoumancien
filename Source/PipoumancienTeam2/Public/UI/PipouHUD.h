// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PipouHUD.generated.h"

class UInputAction;
class USlot;
class UResurrectionWidget;
class UPipouCharacterInputData;

UCLASS()
class PIPOUMANCIENTEAM2_API APipouHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category="Input Data")
	UPipouCharacterInputData* InputData;
	
	UPROPERTY(EditAnywhere, Category="Resurrection")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	void AddWBPResurrection(APlayerController* PlayerController);
	void RemoveResurrection();

	// WBP Slot
	UPROPERTY(EditAnywhere, Category="Resurrection")
	TSubclassOf<USlot> WBPSlotClass;
	
	UPROPERTY()
	USlot* WBPSlotInstance;
	
	void AddWbpSlotInstance(APlayerController* PlayerController, float InputPitch, UInputAction* InputAction);

private:
	// Utilities Functions
	EMusicNoteType GetMusicNoteTypeFromInputAction(UInputAction* InputAction);
	
};
