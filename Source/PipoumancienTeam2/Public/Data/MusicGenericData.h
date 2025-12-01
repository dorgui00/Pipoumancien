// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MusicGenericData.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API UMusicGenericData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// ---- MUSIC DATA ----
	UPROPERTY(EditAnywhere)
	float MusicGlobalSpeed = 1.f;

	UPROPERTY(EditAnywhere)
	int MaxFailNotePossible = 5;

	UPROPERTY(EditAnywhere)
	float TimeTolerance = 0.2;

	
	// ---- SOUNDS ----
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundCue> FailedNoteSound;
	
};
