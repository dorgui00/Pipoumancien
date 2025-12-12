// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BirdData.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UBirdData : public UDataAsset
{
	GENERATED_BODY()

public :
	// Sound Inputs 
	UPROPERTY(EditAnywhere, Category= "Sounds")
	TObjectPtr<USoundBase> UpSound;
	
	UPROPERTY(EditAnywhere, Category= "Sounds")
	TObjectPtr<USoundBase> RightSound;
	
	UPROPERTY(EditAnywhere, Category= "Sounds")
	TObjectPtr<USoundBase> DownSound;
	
	UPROPERTY(EditAnywhere, Category= "Sounds")
	TObjectPtr<USoundBase> LeftSound;
};
