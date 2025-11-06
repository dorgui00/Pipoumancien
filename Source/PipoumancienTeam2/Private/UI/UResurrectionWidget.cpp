// Fill out your copyright notice in the Description page of Project Settings.


#include "UResurrectionWidget.h"
#include "Components/Slider.h"

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

	EndPointFromInputPitch =
	{
		{ -1.f, EndPoint_1 },
		{ -0.5f, EndPoint_2 },
		{ 0.f, EndPoint_3 }, 
		{ 0.5f, EndPoint_4 }, 
		{ 1.f, EndPoint_5 }, 
	};

}

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
