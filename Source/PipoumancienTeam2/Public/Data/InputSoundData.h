// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputSoundData.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UInputSoundData : public UDataAsset
{
	GENERATED_BODY()

	// Sound Inputs 
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> UpSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> RightSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> DownSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> LeftSound;

	// Map Sound From Input
	UPROPERTY()
	TMap<TObjectPtr<UInputAction>, TObjectPtr<USoundBase>> WorldSoundFromInput =
	{
		{ InputData->InputNoteY, UpSound.LoadSynchronous()},
		{ InputData->InputNoteB, RightSound.LoadSynchronous() },
		{ InputData->InputNoteA, DownSound.LoadSynchronous() }, 
		{ InputData->InputNoteX, LeftSound.LoadSynchronous() }
	};
};
