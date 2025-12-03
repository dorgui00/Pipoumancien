// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "UI/UResurrectionWidget.h"
#include "SubsystemSettings.generated.h"

class UMusicGenericData;
class UPartitionFinish;
class UMusicNote;

UCLASS(Config=Game, DefaultConfig, meta=(DisplayName="Subsystem Settings"))
class PIPOUMANCIENTEAM2_API USubsystemSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// ---- HUD DATA ----
	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UMusicNote> WBPNoteClass;

	UPROPERTY(Config, EditAnywhere, Category="GlobalHUD")
	TSubclassOf<UPartitionFinish> WBPPartitionFinishClass;

	
	// ---- MUSIC DATA ----
	UPROPERTY(Config, EditAnywhere, Category="MusicSubsystem")
	TSoftObjectPtr<UMusicGenericData> MusicGenericData;
	

	// ---- DATATABLE MANAGER ----
	UPROPERTY(Config, EditAnywhere, Category="MusicDatatableManager")
	TSoftObjectPtr<UDataTable> DT_Music;


	// ---- CAMERA SUBSYSTEM ----
	UPROPERTY(Config, EditAnywhere, Category="GlobalDatatableManager")
	TSoftObjectPtr<UMaterialInterface> InvisibleMaterial;
	
};
