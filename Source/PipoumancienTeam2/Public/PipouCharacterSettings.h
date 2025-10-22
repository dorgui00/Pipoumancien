// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "PipouCharacterInputData.h"
#include "Engine/DeveloperSettings.h"
#include "PipouCharacterSettings.generated.h"

UCLASS(Config=Game, DefaultConfig, meta = (DisplayName = "Pipou Character Settings"))
class PIPOUMANCIENTEAM2_API UPipouCharacterSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	// Inputs
	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	TSoftObjectPtr<UPipouCharacterInputData> InputData;
	
	UPROPERTY(Config, EditAnywhere, Category="Inputs")
	TSoftObjectPtr<UInputMappingContext> InputMappingContext;
};
