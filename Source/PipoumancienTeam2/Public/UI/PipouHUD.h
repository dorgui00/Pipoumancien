// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USlot.h"
#include "Character/PipouCharacterInputData.h"
#include "GameFramework/HUD.h"
#include "PipouHUD.generated.h"

class UCanvasPanelSlot;
class UInputAction;
class USlot;
class UResurrectionWidget;
class UPipouCharacterInputData;

UCLASS()
class PIPOUMANCIENTEAM2_API APipouHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input Data")
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
	
	USlot* AddWbpSlotInstance(APlayerController* PlayerController, float InputPitch, UInputAction* InputAction);

protected:
	virtual void BeginPlay() override;
	
private:
	// Utilities Functions
	UPROPERTY()
	TMap<UInputAction*, EMusicNoteType> MusicNoteFromInputAction;
	
	EMusicNoteType GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const;
	
};
