// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USlot.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalHUDSubsystem.generated.h"

class UInputAction;
class UResurrectionWidget;
class UPipouCharacterInputData;
class UGlobalGameSubsystem;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalHUDSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input Data")
	UPipouCharacterInputData* InputData;
	
	UPROPERTY(EditAnywhere, Category="Resurrection")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	void AddWBPResurrection();
	
	//Resurrection
	void DisplayResurrection();
	void RemoveResurrection();

	// WBP Slot
	UPROPERTY(EditAnywhere, Category="Resurrection")
	TSubclassOf<USlot> WBPSlotClass;
	
	UPROPERTY()
	USlot* WBPSlotInstance;
	
	void AddWbpSlotInstance(float InputPitch, UInputAction* InputAction);

protected:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
private:
	// Utilities Functions
	UPROPERTY()
	TMap<UInputAction*, EMusicNoteType> MusicNoteFromInputAction;
	
	EMusicNoteType GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const;

	// Game Subsystem
	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	void Init();
};
