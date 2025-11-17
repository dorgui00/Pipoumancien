// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.generated.h"

class UCanvasPanel;
class UImage;
class USlider;

UCLASS()
class PIPOUMANCIENTEAM2_API UResurrectionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	
	// Music
	void SetSliderPitch(float NewPitch) const;

	UPROPERTY()
	TMap<float, UUserWidget*> SpawnPointFromInputPitch;

	UUserWidget* GetSpawnPointFromInputPitch(float InputPitch);

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* SlotSpawnPoints;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* NotesBox;

	UPROPERTY(meta= (BindWidget))
	UCanvasPanel* PartitionBox;

	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* SliderBox;
	
	UPROPERTY(meta = (BindWidget))
	USlider* PitchSlider;

private:
	// Music
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
