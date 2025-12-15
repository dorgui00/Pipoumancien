// Fill out your copyright notice in the Description page of Project Settings.


#include "UResurrectionWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Slider.h"

// ---- USER WIDGET FUNCTIONS ----
void UResurrectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TargetWBPAlpha = 1.f;
	
	SpawnPointFromInputPitch =
	{
		{ -1.f, SpawnPoint_1 },
		{ -0.5f, SpawnPoint_2 },
		{ 0.f, SpawnPoint_3 }, 
		{ 0.5f, SpawnPoint_4 }, 
		{ 1.f, SpawnPoint_5 }, 
	};

	FeedbackPosFromInputPitch =
	{
		{ -1.f, NoteFeedBackOne },
		{ -0.5f, NoteFeedBackTwo },
		{ 0.f, NoteFeedBackThree }, 
		{ 0.5f, NoteFeedBackFour }, 
		{ 1.f, NoteFeedBackFive }, 
	};
}

void UResurrectionWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Manage the slider smoothness
	if (PitchSlider == nullptr) return;
	CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitch, InDeltaTime, InterSpeed);
	PitchSlider->SetValue(CurrentPitch);

	// Change opacity for the widget
	if (CurrentWBPAlpha != TargetWBPAlpha)
	{
		CurrentWBPAlpha = FMath::FInterpTo(CurrentWBPAlpha, TargetWBPAlpha, InDeltaTime, WBPAlphaInterpolation);
		this->SetColorAndOpacity({ GetColorAndOpacity().R, GetColorAndOpacity().G, GetColorAndOpacity().B, CurrentWBPAlpha } );
	}
}


// ---- SLIDER MUSIC ----
void UResurrectionWidget::SetSliderPitch(float NewPitch)
{
	TargetPitch = NewPitch;
}

// ---- NOTES SPAWN POINTS ----
UUserWidget* UResurrectionWidget::GetSpawnPointFromInputPitch(float InputPitch)
{
	return SpawnPointFromInputPitch[InputPitch];
}

void UResurrectionWidget::PlaySliderFailAnimation(float PlaybackSpeed)
{
	PlayAnimation(SliderFail, 0, 1, EUMGSequencePlayMode::Forward, PlaybackSpeed);
}

// ---- ANIMATION POP UI ----
void UResurrectionWidget::SetWBPAlphaToZero()
{
	TargetWBPAlpha = 0;
}


// ---- FEEDBACK NOTES UI ----
UImage* UResurrectionWidget::GetFeedbackPosFromInputPitch(float InputPitch)
{
	return FeedbackPosFromInputPitch[InputPitch];
}
