// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIAnimation.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UUIAnimation : public UUserWidget
{
	GENERATED_BODY()


public:
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* AnimationHelp;

protected:
	
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float DeltaTime) override;
	
};
