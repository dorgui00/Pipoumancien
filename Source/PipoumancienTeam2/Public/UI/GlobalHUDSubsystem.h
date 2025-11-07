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
	UPROPERTY()
	TSoftObjectPtr<UPipouCharacterInputData> InputData;
	
	UPROPERTY()
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	// Resurrection
	void DisplayResurrectionWidget();
	void RemoveResurrectionWidget();

	// WBP Slot
	UPROPERTY()
	TSubclassOf<USlot> WBPNoteClass;
	
	UPROPERTY()
	USlot* WBPNoteInstance;
	
	void SpawnNotesPartition(F_Skeleton* CurrentSkeleton);

	void MovePartition(float DeltaTime);

	// TO EDIT les mettre dans les settings
	// DistanceBetweenNote = Frequency/RatioDistance
	// Our RatioDistance
	float RatioDistance = 0.01f;
	float UiOffset;
	float Timer = 0.f;

	float StartPointLerp = 0.f;
	float EndPointLerp = 0.f;

	UPROPERTY()
	UCanvasPanelSlot* NotesBoxSlot;

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
