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
	// ---- USER WIDGET FUNCTIONS ----
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
	
	// ---- NOTES ----
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* NotesBox;


	// ---- PARTITION ----
	UPROPERTY(meta= (BindWidget))
	UCanvasPanel* PartitionBox;

	UPROPERTY(meta= (BindWidget))
	UImage* PitchMinusOne;

	
	// ---- FEEDBACK NOTES UI ----
	UPROPERTY()
	TMap<float, UImage*> FeedbackPosFromInputPitch;

	UImage* GetFeedbackPosFromInputPitch(float InputPitch);

	
	// ---- SLIDER MUSIC ----
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* SliderBox;
	
	UPROPERTY(meta = (BindWidget))
	USlider* PitchSlider;

	float TargetPitch = 0.f;
	float CurrentPitch = 0.f;

	UPROPERTY()
	float InterSpeed = 8.f;

	void SetSliderPitch(float NewPitch);


	// ---- NOTES SPAWN POINTS ----
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* SlotSpawnPoints;
	
	UPROPERTY()
	TMap<float, UUserWidget*> SpawnPointFromInputPitch;

	UUserWidget* GetSpawnPointFromInputPitch(float InputPitch);

	
	// ---- ANIMATION POP UI ----
	void SetWBPAlphaToZero();

private:
	// ---- NOTES SPAWN POINTS ----
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

	// ---- FEEDBACK NOTES UI ----
	UPROPERTY(meta = (BindWidget))
	UImage* NoteFeedBackOne;

	UPROPERTY(meta = (BindWidget))
	UImage* NoteFeedBackTwo;

	UPROPERTY(meta = (BindWidget))
	UImage* NoteFeedBackThree;

	UPROPERTY(meta = (BindWidget))
	UImage* NoteFeedBackFour;

	UPROPERTY(meta = (BindWidget))
	UImage* NoteFeedBackFive;

	// ---- ANIMATION POP UI ----
	float TargetWBPAlpha = 0.f;
	float CurrentWBPAlpha = 0.f;
	float WBPAlphaInterpolation = 2.f;
	
};
