// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MusicGenericData.generated.h"

class USoundCue;

UCLASS()
class PIPOUMANCIENTEAM2_API UMusicGenericData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// ---- MUSIC DATA ----
	UPROPERTY(EditAnywhere)
	int MaxFailNotePossible = 5;

	UPROPERTY(EditAnywhere)
	float TimeTolerance = 0.2;

	UPROPERTY(EditAnywhere)
	float PitchTolerance = 0.2f;

	UPROPERTY(EditAnywhere)
	float RatioDistance = 150.f; 

	
	// ---- SOUNDS ----
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundCue> FailedNoteSound;
	
};
