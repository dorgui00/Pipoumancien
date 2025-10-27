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
	static AMusicManager* Instance();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void InitMusicManager(F_Skeleton* Skeleton);

	F_Note* GetWaitingNote();

	F_Note* CurrentWaitingNote = nullptr;
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResetReplyEvent);

	UPROPERTY()
	FResetReplyEvent ResetReplyEvent;

	float CurrentCursorValue = 0.f;
	
protected:
	F_Skeleton* CurrentSkeleton = nullptr;
	
	bool IsInWorldStateMusic = false;
	bool IsInCountDown = false;
	bool IsAwaitingReply = false;
	
	float TimerCountDown = 0.f;
	float Tempo = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float Tolerance = 0.1f;

	void StartCountDown();
	
private :
	static AMusicManager* MyInstance;

	int CurrentWaitingNoteIndex = 0;
	
	
};
