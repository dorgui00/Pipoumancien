// Fill out your copyright notice in the Description page of Project Settings.


#include "UResurrectionWidget.h"
#include "Components/Slider.h"

// Music
void UResurrectionWidget::SetSliderPitch(float NewPitch) const
{
	if (PitchSlider == nullptr) return;
	PitchSlider->SetValue(NewPitch);
}

UUserWidget* UResurrectionWidget::GetSpawnPointFromInputPitch(float InputPitch)
{
	return SpawnPointFromInputPitch[InputPitch];
}

UUserWidget* UResurrectionWidget::GetEndPointFromInputPitch(float InputPitch)
{
	return EndPointFromInputPitch[InputPitch];
}
