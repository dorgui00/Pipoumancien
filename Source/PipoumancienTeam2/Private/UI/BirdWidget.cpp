// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BirdWidget.h"

void UBirdWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("Image01 = %s"), Image01 ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Image02 = %s"), Image02 ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Image03 = %s"), Image03 ? TEXT("OK") : TEXT("NULL"));
	Images = {Image01,Image02,Image03};
	AnimationToucheIdel();
	
}

void UBirdWidget::AnimationToucheIdel()
{
	PlayAnimation(AnimationTouche, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche1V()
{
	PlayAnimation(Touche1Valide, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche2V()
{
	PlayAnimation(Touche2Valide, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche3V()
{
	PlayAnimation(Touche3Valide, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche1F()
{
	PlayAnimation(Touche1Faile, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche2F()
{
	PlayAnimation(Touche1Faile, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::Touche3F()
{
	PlayAnimation(Touche1Faile, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}
