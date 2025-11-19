// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MusicWorldSubsystem.generated.h"

class ASkeletonController;
class UGlobalHUDSubsystem;
class UGlobalGameSubsystem;
struct F_Note;
struct F_Skeleton;

UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API UMusicWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	#pragma region Timer Music
	// Timer we increment while the UI is lerping.
	float TimerLerpingOffset = 0.f;
	
	#pragma endregion

	#pragma region Skeleton&Notes
	// Get the current note playing.
	F_Note* GetCurrentWaitingNote() const;

	// Manage the WaitingNoteIndex.
	int GetCurrentWaitingNoteIndex() const;
	void SetCurrentWaitingNoteIndex(int NewIndex);

	// Change variable HasMusicianReceivedInput to true if not.
	void ReceivedMusicianInput();
	
	#pragma endregion

	#pragma region Music
	// Music Speed.
	UPROPERTY()
	float MusicGlobalSpeed = 1.f;

	// Tolerance for the player to play the QTE.
	float TimeTolerance = 0.2f;

	// Check if the UI is lerping offset.
	bool IsLerpingOffset = true;

	// Store the current cursor value.
	float CurrentCursorValue = 0.f;

	// When you're not at the right time for the QTE.
	void LostQTE();
	
	#pragma endregion

	#pragma region Replies
	bool IsAwaitingReply = false;
	
	#pragma endregion

	#pragma region Countdown
	bool IsInCountDown = false;
	
	#pragma endregion

	#pragma region Utilities
	void InitMusic(ASkeletonController* Skeleton);

	#pragma endregion
	
protected:
	#pragma region SubsystemOverride
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};

	#pragma endregion

private:
	#pragma region Countdown
	float TimerCountDown = 3.f;

	void StartCountDown();
	
	#pragma endregion

	#pragma region Skeleton&Notes
	UPROPERTY()
	ASkeletonController* CurrentSkeleton = nullptr;
	
	UPROPERTY()
	bool HasMusicianReceivedInput = false;

	UPROPERTY()
	bool IsConductorOnTheRightPitch = false;

	int CurrentWaitingNoteIndex = 0;

	void ResetMusicianReply();
		
	#pragma endregion

	#pragma region Music
	// Tempo handle all the music rythm.
	float Tempo = 0.f;
	
	void FinishMelody();

	float PitchTolerance = 0.2f;

	bool HasAchievedQte();


	#pragma endregion

	#pragma region Utilities
	bool IsInWorldStateMusic = false;

	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

	#pragma endregion

};
