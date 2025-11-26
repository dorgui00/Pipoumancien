// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MusicWorldSubsystem.generated.h"

class UMusicNote;
class ASkeletonController;
class UGlobalHUDSubsystem;
class UGlobalGameSubsystem;
struct F_Note;
struct F_Skeleton;

enum class EMelodyType : uint8
{
	NONE,
	SUCCEED,
	FAILED
};

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

	UMusicNote* GetCurrentWaitingNoteWBP() const; 

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

	UPROPERTY()
	int CurrentFailNotePossible;

	UPROPERTY()
	int MaxFailNotePossible = 5;

	// Tolerance for the player to play the QTE.
	float TimeTolerance = 0.2f;

	// Check if the UI is lerping offset.
	bool IsLerpingOffset = true;

	// Store the current cursor value.
	float CurrentCursorValue = 0.f;

	// When you're not at the right time for the QTE.
	void LostQTE();

	void LostMelody();

	void SetNoteFeedbackMusic(FLinearColor NewColor) const;

	EMelodyType GetMelodyType() const;
	
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
	
	void SucceedMelody();

	float PitchTolerance = 0.2f;

	bool HasAchievedQte();

	EMelodyType MelodyState = EMelodyType::NONE;

	bool HasLostMelody = false;

	#pragma endregion

	#pragma region Utilities
	bool IsInWorldStateMusic = false;

	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

	#pragma endregion

};
