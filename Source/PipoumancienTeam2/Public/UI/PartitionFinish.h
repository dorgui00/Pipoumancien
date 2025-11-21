// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PartitionFinish.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UPartitionFinish : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WinLoseText;

	TObjectPtr<UTextBlock> GetWinLoseText();
	void SetWinLostText(const FString& NewText) const;

	// Animation Widget
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> PartitionFinishAnimationEnter;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> PartitionFinishAnimationExit;

	UFUNCTION()
	void PlayAnimationPartitionFinishStart();

	UFUNCTION()
	void PlayAnimationPartitionFinishExit();
	
	
private:
};
