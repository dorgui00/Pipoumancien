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

public :
	// Spawn Points
	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox01;

	UPROPERTY(meta = (BindWidget))
	UImage* Image01;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox02;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Image02;

	UPROPERTY(meta = (BindWidget))
	UScaleBox* ScaleBox03;

	UPROPERTY(meta = (BindWidget))
	UImage* Image03;

	
};
