// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkeletonInteractionWidget.generated.h"

class UImage;
class UInputAction;
class UScaleBox;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API USkeletonInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

	// Spawn Points
	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox01;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox02;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox03;

	
};
