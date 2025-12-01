// Fill out your copyright notice in the Description page of Project Settings.


#include "UResurrectionWidget.h"
#include "Components/Slider.h"

// ---- USER WIDGET FUNCTIONS ----
void UResurrectionWidget::NativeConstruct()
{
	Super::NativeConstruct();

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

	if (PitchSlider == nullptr) return;
	CurrentPitch = FMath::FInterpTo(CurrentPitch, TargetPitch, InDeltaTime, InterSpeed);
	PitchSlider->SetValue(CurrentPitch);
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


// ---- FEEDBACK NOTES UI ----
UImage* UResurrectionWidget::GetFeedbackPosFromInputPitch(float InputPitch)
{
	return FeedbackPosFromInputPitch[InputPitch];
}
