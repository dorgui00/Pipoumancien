// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.generated.h"

class USlider;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UResurrectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Par exemple, une fonction d'initialisation que tu pourras appeler dans Blueprint ou en C++
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitWidget();

	// Music
	UPROPERTY(meta = (BindWidget))
	USlider* PitchSlider;
	
	void SetSliderPitch(float NewPitch);

};
