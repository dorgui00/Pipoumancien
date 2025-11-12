// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "MusicWorldSubsystem.generated.h"

class ASkeletonController;
class UGlobalHUDSubsystem;
class UMusicTimeline;
class UTimelineComponent;
class UGlobalGameSubsystem;
struct F_Note;
struct F_Skeleton;

UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API UMusicWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
#pragma region SubsystemOverride
protected:
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};
	
	
#pragma endregion
	
#pragma region Timer
public:
	UPROPERTY(EditDefaultsOnly)
	float Speed = 1.f;
	
	float Tempo = 0.f;

private:
	bool IsInCountDown = false;
	
	float TimerCountDown = 3.f;
	
	UPROPERTY(EditDefaultsOnly)
	float TimeTolerance = 0.2f;

	void StartCountDown();
	
#pragma endregion

#pragma region Skeleton&Notes
	
public :
	F_Note* GetWaitingNote() const;
	
	void ReceivedMusicianInput();


	
private :
	UPROPERTY()
	ASkeletonController* CurrentSkeleton = nullptr;

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
	
	bool HasAchievedQte() const;
	
	int Replies = 0;

	void ResetMusicianReply();
	
#pragma endregion

#pragma region Misc
	
public :
	void InitMusic(ASkeletonController* Skeleton);
	
	float CurrentCursorValue = 0.f;
	
private :
	bool IsInWorldStateMusic = false;

	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

	void EndMelody();

#pragma endregion
};
