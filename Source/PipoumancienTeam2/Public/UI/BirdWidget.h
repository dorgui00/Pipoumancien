// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BirdWidget.generated.h"

class UImage;
class UScaleBox;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UBirdWidget : public UUserWidget
{
	GENERATED_BODY()

public :
	// ---- USER WIDGET FUNCTIONS ----
	virtual void NativeConstruct() override;
	
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

	UPROPERTY()
	TArray<UImage*> Images;

	void AnimationToucheIdel();
	void ToucheV();
	void ToucheF();
	void RemoveWidget();
	void AnimationPlayIdel();

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimationTouche;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Touche1Valide;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Touche1Faile;
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* RemoveAnimation;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimationIdel;
	
	

};
