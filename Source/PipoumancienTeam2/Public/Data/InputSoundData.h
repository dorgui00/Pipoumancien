// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputSoundData.generated.h"

/**
 * 
 */
class USoundBase;

UCLASS()
class PIPOUMANCIENTEAM2_API UInputSoundData : public UDataAsset
{
	GENERATED_BODY()

public :
	// Sound Inputs 
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> UpSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> RightSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> DownSound;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LeftSound;

};
