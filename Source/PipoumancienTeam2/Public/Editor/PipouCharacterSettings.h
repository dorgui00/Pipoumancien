// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "Character/PipouCharacterInputData.h"
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

	// Sounds
	UPROPERTY(Config, EditAnywhere, Category="Sounds")
	TSoftObjectPtr<UInputSoundData> InputSoundData;

	// Sound Inputs 
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> UpSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> RightSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> DownSound;
	
	UPROPERTY(Config, EditAnywhere, Category="Sound Inputs")
	TSoftObjectPtr<USoundBase> LeftSound;

	// map sound from input
	void InitWorldSounds();
	
	TObjectPtr<USoundBase> GetWorldSoundFromInput(TObjectPtr<UInputAction>) const ;
	
private :
	UPROPERTY()
	TMap<TObjectPtr<UInputAction>, TObjectPtr<USoundBase>> WorldSoundFromInput;

};
