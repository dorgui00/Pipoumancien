// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USlot.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalHUDSubsystem.generated.h"

struct F_Skeleton;
class UInputAction;
class UResurrectionWidget;
class UPipouCharacterInputData;
class UGlobalGameSubsystem;


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

	// Resurrection
	void DisplayResurrectionWidget();
	void RemoveResurrectionWidget();

	// WBP Slot
	UPROPERTY(EditAnywhere, Category="Resurrection")
	TSubclassOf<USlot> WBPNoteClass;
	
	UPROPERTY()
	USlot* WBPNoteInstance;
	
	void SpawnNotesPartition(F_Skeleton* CurrentSkeleton);

	// DistanceBetweenNote = Frequency/RatioDistance
	// Our RatioDistance
	float RatioDistance = 0.01f;

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
