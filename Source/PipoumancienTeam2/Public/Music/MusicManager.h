// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MusicManager.generated.h"

/**
 * 
 */
struct F_Note;
struct F_Skeleton;

UCLASS()
class PIPOUMANCIENTEAM2_API AMusicManager : public AActor
{
	GENERATED_BODY()
	
public :
#pragma region Override
	
	AMusicManager();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;
	
#pragma endregion
	
#pragma region Instance
public :
	static AMusicManager* Instance(UWorld* World);
private :
	static AMusicManager* MyInstance;
#pragma endregion

#pragma region Timer
private :
	bool IsInCountDown = false;
	
	float TimerCountDown = 3.f;
	float Tempo = 0.f;

	void StartCountDown();
	
#pragma endregion

#pragma region Skeleton&Notes
	
public :
	F_Note* GetWaitingNote();
	
	void CheckReceivedInput();
	
private :
	F_Skeleton* CurrentSkeleton = nullptr;
	
	UPROPERTY(EditDefaultsOnly)
	float Tolerance = 0.5f;

	UPROPERTY()
	bool HasMusicianReceivedInput = false;
	
	UPROPERTY(EditDefaultsOnly)
	float PitchTolerance = 0.1f;

	int CurrentWaitingNoteIndex = 0;
	
#pragma endregion
	
#pragma region Replies
public :
	
	bool IsAwaitingReply = false;

	void ReceiveInput();
	
private :
	bool HasAchievedQte();
	
	int Replies = 0;

	void ResetReplies();
	
#pragma endregion

#pragma region Misc
	
public :
	void InitMusicBySkeleton(F_Skeleton* Skeleton);
	
	float CurrentCursorValue = 0.f;
	
private :
	bool IsInWorldStateMusic = false;

	bool HasPrint = false;

#pragma endregion
	
};
