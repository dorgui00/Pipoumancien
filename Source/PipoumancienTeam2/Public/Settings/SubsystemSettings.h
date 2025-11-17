// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/UResurrectionWidget.h"
#include "SubsystemSettings.generated.h"

class UMusicNote;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Subsystem Settings"))
class PIPOUMANCIENTEAM2_API USubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	#pragma region HUDSubsystem
	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UMusicNote> WBPNoteClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	float RatioDistance = 100.f;
	
	#pragma endregion

	#pragma region MusicSubsystem
	UPROPERTY(Config, EditAnywhere, Category="MusicSubsystem")
	float MusicGlobalSpeed = 1.f;

	UPROPERTY(Config, EditAnywhere, Category="MusicSubsystem")
	float TimeTolerance = 0.3f;
	
	#pragma endregion

	#pragma region DataManager
	UPROPERTY(Config, EditAnywhere, Category="GlobalDatatableManager")
	TSoftObjectPtr<UDataTable> DT_Music;

	#pragma endregion
};
