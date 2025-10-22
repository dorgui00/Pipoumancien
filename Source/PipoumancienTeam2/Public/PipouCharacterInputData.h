// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PipouCharacterInputData.generated.h"

class UInputAction;

UCLASS()
class PIPOUMANCIENTEAM2_API UPipouCharacterInputData : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputActionMoveXY;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputPitch;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputNoteA;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputNoteB;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputNoteX;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> InputNoteY;
};
