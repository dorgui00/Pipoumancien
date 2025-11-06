// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/UResurrectionWidget.h"
#include "SubsystemSettings.generated.h"

/**
 * 
 */

class USlot;
class UPipouCharacterInputData;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Subsystem Settings"))
class PIPOUMANCIENTEAM2_API USubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// HUDSubsystem
	UPROPERTY(Config, EditAnywhere, Category="HUD")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	// UPROPERTY(Config, EditAnywhere, Category="HUD")
	// UPipouCharacterInputData* InputData;

	UPROPERTY(Config, EditAnywhere, Category="HUD")
	TSubclassOf<USlot> WBPNoteClass;

	//others
};
