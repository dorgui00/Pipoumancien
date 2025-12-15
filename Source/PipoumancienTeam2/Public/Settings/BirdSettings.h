// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "BirdSettings.generated.h"

class UBirdData;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Bird Settings"))
class PIPOUMANCIENTEAM2_API UBirdSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public :
	// Sounds
	UPROPERTY(Config, EditAnywhere, Category="Sounds")
	TSoftObjectPtr<UBirdData> BirdData;
};
