// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMusicNote.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/TextBlock.h"
#include "GlobalHUDSubsystem.generated.h"

class UHUDData;
class UPartitionFinish;
struct F_Note;
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
	// ---- MUSIC UI ----
	UPROPERTY()
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	UPROPERTY()
	TSubclassOf<UPartitionFinish> WBPPartitionFinishClass;

	UPROPERTY()
	TObjectPtr<UPartitionFinish> WBPPartitionFinishInstance;
	
	UPROPERTY()
	TSubclassOf<UMusicNote> WBPNoteClass;

	UPROPERTY()
	UMusicNote* WBPNoteInstance;

	// Create or Remove the widget of the resurrection.
	void DisplayResurrectionWidget();
	void RemoveResurrectionWidget();

	// Create or Remove the widget of feedback of the finish partition.
	void DisplayPartitionFinish(const FString& TextInThePartition);
	void RemovePartitionFinish() const;
	
	float GetUISpeed() const;
	float GetUIOffset() const;

	// ---- MUSIC UI NOTES ----
	// Array of Note Spawned.
	UPROPERTY()
	TArray<UMusicNote*> NotesInstanciated;

	// Store the distance of all the frequencies when spawning notes.
	float DistancePreviousFrequencies;

	float TimePreviousFrequencies;

	// Canvas Panel Slot of the Note Instance. 
	UPROPERTY()
	UCanvasPanelSlot* NotesBoxSlot;
	
	// Spawn the note in the NoteBox of the partition from the Skeleton Current Notes.
	void SpawnNotesPartition(const ASkeletonController* CurrentSkeleton);

	// Move the partition at the same time of the main music mechanic in MusicWorldSubsystem.
	void MovePartition(float DeltaTime);
	
	
	// ---- UTILITIES ----
	// Use to store the Input Data
	UPROPERTY()
	TObjectPtr<UPipouCharacterInputData> InputData;

	UPROPERTY()
	TObjectPtr<UHUDData> HUDData;

	UPROPERTY()
	TMap<UInputAction*, UImage*> ImageFromNoteInput;

	void SetMusicWorldSubsystem(UMusicWorldSubsystem* NewMusicSubsystem);

	// ---- FEEDBACK COLORS NOTES ----
	float TimerBeforeResetingColor = 0;

	// Timer event to reset the colors after a number of seconds.
	FTimerHandle ResetColorTimerHandle;

	// Associate an Object to a timer Event.
	UPROPERTY()
	TMap<UObject*, FTimerHandle> ColorResetTimers;

	void SetImageColor(UImage* CurrentImage, FLinearColor NewColor);

	// UI WORLD
	void DisplayNotesForSkeletonInteraction(UInputAction* InputAction);

	void SpawnSkeletonInteractionWidget();
	
protected:
	// ---- GAME INSTANCE SUBSYSTEM ----
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	
	// ---- FTickableGameObject ----
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); };
	
private:

	// ---- MUSIC UI ----
	float UISpeed = 0;
	
	// Size of the partition UI.
	float UIOffset;


	// ---- MUSIC UI NOTES ----
	// Moving Start and End Point of the NotesBox.
	float MovementStartPoint = 0.f;
	float MovementEndPoint = 0.f;

	
	// ---- FEEDBACK COLORS NOTES ----
	void Internal_SetImageColor(UImage* CurrentImage, FLinearColor NewColor);

	
	// ---- UTILITIES ----
	UPROPERTY()
	UMusicWorldSubsystem* MusicWorldSubsystem;

	// Unit of Distance.
	float RatioDistance = 300.f;
	
	// Map to associate an InputAction (Key of Controller) to a MusicNoteType (The notes in ENUM).
	UPROPERTY()
	TMap<UInputAction*, EMusicNoteType> MusicNoteFromInputAction;

	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;
	
	// Initialize data for GlobalHUDSubsystem.
	void Init();
	
	// Get the MusicNoteType with a key input action based on the map.
	EMusicNoteType GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const;

};
