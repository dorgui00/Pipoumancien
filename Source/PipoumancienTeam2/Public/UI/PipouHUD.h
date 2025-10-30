// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PipouHUD.generated.h"

class UResurrectionWidget;

UCLASS()
class PIPOUMANCIENTEAM2_API APipouHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UResurrectionWidget> WBPResurrectionClass;

	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	void AddWBPResurrection(APlayerController* PlayerController);
};
