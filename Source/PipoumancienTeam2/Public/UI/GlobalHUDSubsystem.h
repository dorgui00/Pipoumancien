// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "USlot.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalHUDSubsystem.generated.h"

class USlider;
class UMusicWorldSubsystem;
class ASkeletonController;
struct F_Skeleton;
class UInputAction;
class UResurrectionWidget;
class UPipouCharacterInputData;
class UGlobalGameSubsystem;

UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalHUDSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	/// Music
	// Resurrection Widget Class
	UPROPERTY()
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	// Resurrection Widget Object (Use to store the creation of the widget with the Resurrection Widget Class)
	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	// Display the resurrection widget to the screen.
	void DisplayResurrectionWidget();

	// Remove the resurrection widget from screen.
	void RemoveResurrectionWidget();

	// Note Widget Class
	UPROPERTY()
	TSubclassOf<USlot> WBPNoteClass;

	// Note Widget Object (Use to store the creation of the widget with the Note Widget Class)
	UPROPERTY()
	USlot* WBPNoteInstance;

	// Spawn the note in the NoteBox of the partition from the Skeleton Current Notes.
	void SpawnNotesPartition(const ASkeletonController* CurrentSkeleton);

	// Move the partition at the same time of the main music mechanic in MusicWorldSubsystem.
	void MovePartition(float DeltaTime);

	// Move UI backwards depending on the position of the note the player has to play (Only if he lost the qte) 
	void RewindPartition(int NoteIndex);

	// TO EDIT les mettre dans les settings
	// DistanceBetweenNote = Frequency/RatioDistance
	// Our RatioDistance
	float RatioDistance = 100.f;
	float UiOffset;
	float Timer = 0.f;

	float PosXDeux = 0;

	float StartPointLerp = 0.f;
	float EndPointLerp = 0.f;
	float UISpeed = 0.f;
	float UiOffsetInTime = 0.f;

	float DistancePreviousFrequencies;

	// Store the note widget spawned.
	TArray<USlot*> NotesInstanciated;

	// Canvas Panel Slot of the Note Instance. 
	UPROPERTY()
	UCanvasPanelSlot* NotesBoxSlot;

	// Set MusicWorldSubsystem from MusicWorldSubsystem. 
	UPROPERTY()
	UMusicWorldSubsystem* MusicWorldSubsystem;

	/// Utilities
	// Use to store the Input Data
	UPROPERTY()
	TObjectPtr<UPipouCharacterInputData> InputData;
	
	// Set Color
	float TimerForResetingColor = 0;

	template<class T>
	void SetObjectColor(T CurrentObject, FLinearColor NewColor)
	{
		if constexpr (std::is_same_v<T, USlot*>)
		{
			CurrentObject->SetColorAndOpacity(NewColor);
		}

		if constexpr (std::is_same_v<T, USlider*>)
		{
			CurrentObject->SetSliderHandleColor(NewColor);
		}
	
		TimerForResetingColor = 0.5f;
	}
	
protected:
	// Default Unreal Functions
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Tickable
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); };
	
private:
	/// Utilities Functions
	// Map to associate an InputAction (Key of Controller) to a MusicNoteType (The notes in ENUM).
	UPROPERTY()
	TMap<UInputAction*, EMusicNoteType> MusicNoteFromInputAction;

	// Get the MusicNoteType with a key input action based on the map.
	EMusicNoteType GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const;

	// Game Subsystem
	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	// Initialize data for GlobalHUDSubsystem
	void Init();

};
