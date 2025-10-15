// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "PlayerSettings.generated.h"

class APipouCharacter;
/**
 * 
 */
UCLASS(Config=Game, DefaultConfig, meta= (DisplayName="Player Settings"))
class PIPOUMANCIENTEAM2_API UPlayerSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, Category="Characters")
	TSubclassOf<APipouCharacter> PipouCharacterClassP0;

	UPROPERTY(Config, EditAnywhere, Category="Characters")
	TSubclassOf<APipouCharacter> PipouCharacterClassP1;
};
