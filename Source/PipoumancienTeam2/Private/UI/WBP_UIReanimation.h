// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WBP_UIReanimation.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UWBP_UIReanimation : public UUserWidget
{
	GENERATED_BODY()

public:
	// Par exemple, une fonction d'initialisation que tu pourras appeler dans Blueprint ou en C++
	UFUNCTION(BlueprintCallable, Category = "UI")
	void InitWidget();
	
};
