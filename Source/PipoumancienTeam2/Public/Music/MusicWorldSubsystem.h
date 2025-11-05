// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GlobalGameSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "MusicWorldSubsystem.generated.h"

/**
 * 
 */

UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API UMusicWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
#pragma region SubsystemOverride
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};
	
#pragma endregion
	
#pragma region Timer
private :
	bool IsInCountDown = false;
	
	float TimerCountDown = 3.f;
	float Tempo = 0.f;
	
	UPROPERTY(EditDefaultsOnly)
	float TimeTolerance = 0.8f;

	void StartCountDown();
	
#pragma endregion

#pragma region Skeleton&Notes
	
public :
	F_Note* GetWaitingNote() const;
	
	void ReceivedMusicianInput();
	
private :
	F_Skeleton* CurrentSkeleton = nullptr;

	UPROPERTY()
	bool HasMusicianReceivedInput = false;

	int CurrentWaitingNoteIndex = 0;
	
#pragma endregion
	
#pragma region Replies
public :
	
	bool IsAwaitingReply = false;
	
private :
	UPROPERTY(EditDefaultsOnly)
	float PitchTolerance = 0.1f;
	
	bool HasAchievedQte();
	
	int Replies = 0;

	void ResetMusicianReply();
	
#pragma endregion

#pragma region Misc
	
public :
	void InitMusic(F_Skeleton* Skeleton);
	
	float CurrentCursorValue = 0.f;
	
private :
	bool IsInWorldStateMusic = false;

	UGlobalGameSubsystem* GlobalGameSubsystem;

#pragma endregion
};
