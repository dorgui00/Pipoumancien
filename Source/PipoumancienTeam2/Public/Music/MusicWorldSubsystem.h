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
	#pragma region Timer

	// Music Speed.
	UPROPERTY()
	float MusicGlobalSpeed = 0.f;

	// Tempo handle all the music rythm.
	float Tempo = 0.f;

	// Tolerance for the player to play the QTE.
	float TimeTolerance = 0.f;

	// Check if the UI is lerping offset.
	bool IsLerpingOffset = true;

	// Timer we increment while the UI is lerping.
	float TimerLerpingOffset = 0.f;
	
	#pragma endregion

	#pragma region Skeleton&Notes
	F_Note* GetWaitingNote() const;
	
	void ReceivedMusicianInput();
	
	#pragma endregion

	#pragma region Replies
	bool IsAwaitingReply = false;
	
	#pragma endregion

	#pragma region Misc
	void InitMusic(ASkeletonController* Skeleton);
	
	float CurrentCursorValue = 0.f;

	#pragma endregion
	
protected:
	#pragma region SubsystemOverride
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};

	#pragma endregion

private:
	#pragma region Timer
	bool IsInCountDown = false;
	
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
		
	#pragma endregion

	#pragma region Replies
	UPROPERTY(EditDefaultsOnly)
	float PitchTolerance = 0.2f;
	
	bool HasAchievedQte();
	
	int Replies = 0;

	void ResetMusicianReply();

	#pragma endregion

	#pragma region Misc
	bool IsInWorldStateMusic = false;

	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

	void FinishMelody();

	#pragma endregion

};
