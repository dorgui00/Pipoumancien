// Fill out your copyright notice in the Description page of Project Settings.


#include "UResurrectionWidget.h"
#include "Components/Slider.h"


void UResurrectionWidget::InitWidget()
{
	
}

// Music
void UResurrectionWidget::SetSliderPitch(float NewPitch)
{
	if (PitchSlider == nullptr) return;
	PitchSlider->SetValue(NewPitch);
}
