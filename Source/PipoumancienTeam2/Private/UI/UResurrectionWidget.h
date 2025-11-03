// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.generated.h"

class UImage;
class USlider;

UCLASS()
class PIPOUMANCIENTEAM2_API UResurrectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Music
	void SetSliderPitch(float NewPitch) const;

	UPROPERTY()
	TMap<float, UUserWidget*> SpawnPointFromInputPitch =
	{
		{ -1.f, SpawnPoint_1 },
		{ -0.5f, SpawnPoint_2 },
		{ 0.f, SpawnPoint_3 }, 
		{ 0.5f, SpawnPoint_4 }, 
		{ 1.f, SpawnPoint_5 }, 
	};

	UUserWidget* GetSpawnPointFromInputPitch(float InputPitch);

private:
	// Music
	UPROPERTY(meta = (BindWidget))
	USlider* PitchSlider;
	
	UPROPERTY(meta = (BindWidget))
	UUserWidget* SpawnPoint_1;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* SpawnPoint_2;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* SpawnPoint_3;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* SpawnPoint_4;

	UPROPERTY(meta = (BindWidget))
	UUserWidget* SpawnPoint_5;
	
};
