// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/UResurrectionWidget.h"
#include "SubsystemSettings.generated.h"

class USlot;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Subsystem Settings"))
class PIPOUMANCIENTEAM2_API USubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:

	// HUDSubsystem
	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<USlot> WBPNoteClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalDatatableManager")
	TSoftObjectPtr<UDataTable> DT_Music;
};
