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
	// ---- MUSIC UI TIMER ----
	float GetTimerLerpingOffset() const;
	void IncreaseTimerLerpingOffset(float DeltaTime);
	
	// ---- MUSIC COUNTDOWN ----
	bool IsInCountDown = false;
	
	float GetTimerCountdown() const;
	void DecreaseTimerCountdown(float DeltaTime);

	
	// ---- NOTES & SKELETONS ----
	F_Note* GetCurrentWaitingNote() const;
	UMusicNote* GetCurrentWaitingNoteWidget() const;

	int GetCurrentWaitingNoteIndex() const;

	int GetCurrentWaitingNoteIndexUI() const;
	F_Note* GetCurrentWaitingNoteUI() const;

	// Tell the Music Logic the player HAS pressed an input.
	void ReceivedMusicianInput();
	
	
	// ---- MUSIC LOGIC ----
	UPROPERTY()
	float MusicGlobalSpeed = 1.f;

	UPROPERTY()
	int MaxFailNotePossible = 5;

	UPROPERTY()
	float TimeTolerance = 0.2f;

	float GetCurrentPitchCursorValue() const;
	void SetCurrentPitchCursorValue(float NewPitchCursorValue);

	// Return the status of IsAwaitingReplyVariable.
	bool GetIsAwatingReply() const;

	// Is the player before the time to do the QTE.
	bool IsBeforeWindowNote() const;

	// Called when the player performs an action too early or too late.
	void LostQTE();
	void LostMelody();
	void SetNoteFeedbackMusic(FLinearColor NewColor) const;

	// Fail Note possible -> Health Bar of the partition.
	int GetCurrentFailNotePossible() const;
	void SetCurrentFailNotePossible(float NewValue);
	bool HasLostAllFaileNotePossible() const;
	bool HasCurrentFailNoteReachMaximumValue() const;
	
	EMelodyType GetMelodyType() const;

	
	// ---- UTILITIES ----
	void InitMusic(ASkeletonController* Skeleton);

	
	// ---- SOUND ----
	UPROPERTY()
	USoundCue* FailedNoteSound;

protected:
	// ---- SUBSYSTEM OVERRIDE ---- 
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};

private:
	// ---- MUSIC UI TIMER ----
	float TimerLerpingOffset = 0.f;
	bool IsLerpingOffset = true;

	UPROPERTY()
	int CurrentWaitingNoteIndexUI = 0;
	bool HasReachFrequency = false;
	
	bool HasFinishedLerpingOffset() const;

	bool HasReachPitchSlider() const;

	
	// ---- MUSIC COUNTDOWN ---- 
	float TimerCountDown = 3.f;
	void StartCountDown();
	void FinishCountDown();
	bool HasFinishedCountdown() const;

	
	// ---- NOTES & SKELETONS ----
	UPROPERTY()
	ASkeletonController* CurrentSkeleton = nullptr;
	
	UPROPERTY()
	bool HasMusicianReceivedInput = false;

	UPROPERTY()
	bool IsConductorOnTheRightPitch = false;

	UPROPERTY()
	int CurrentWaitingNoteIndex = 0;

	// Tell the Music Logic the player HAS NOT pressed an input.
	void ResetMusicianReply();
	
	void SetCurrentWaitingNoteIndex(int NewIndex);

	
	// ---- MUSIC LOGIC ----
	float Tempo = 0.f;
	float TempoNoteUI = 0.f;

	// The slider value accepted 
	float PitchTolerance = 0.2f;

	UPROPERTY()
	float CurrentPitchCursorValue = 0.f;

	bool HasLostMelody = false;
	EMelodyType MelodyState = EMelodyType::NONE;
	
	// The Music Logic is waiting or not for the player to do an action.
	bool IsAwaitingReply = false;

	UPROPERTY()
	int CurrentFailNotePossible;

	void PlayMusic();

	void IncreaseMusicTempo(float DeltaTime);

	// Change the CurrentWaitingNoteIndex to go to the next note.
	void GoNextNote();

	

	// Has the player enter the time to do the QTE.
	bool HasEnteredWindowNote() const;

	// Has the player exited time to do the QTE.
	bool HasExitedWindowNote() const;

	// Has the player play all the notes in the current Notes Array.
	bool HasFinishedMelody() const;

	// Called when the player pressed input during the time of the QTE.
	void SucceedQTE();
	
	void SucceedMelody();
	bool HasAchievedQte();

	
	// ---- UTILITIES ----
	bool IsInWorldStateMusic = false;
	
	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

	
	// ---- SOUND ----
	UPROPERTY()
	UAudioComponent* BackgroundAudioComponent;

};
