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

	FTimerHandle TimerHandle1;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle1,
		[this]()
		{
			
			AnimationPlayIdel();
		},
		2.f,
		false
	);
}

void UBirdWidget::AnimationToucheIdel()
{
	PlayAnimation(AnimationTouche, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::ToucheV()
{
	PlayAnimation(Touche1Valide, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}


void UBirdWidget::ToucheF()
{
	PlayAnimation(Touche1Faile, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UBirdWidget::RemoveWidget()
{
	PlayAnimation(RemoveAnimation, 0.0f, 1, EUMGSequencePlayMode::Forward, 1.0f, false);
	//Images = {Image01,Image02,Image03};
	
}

void UBirdWidget::AnimationPlayIdel()
{
	PlayAnimation(AnimationIdel, 0.0f, 0, EUMGSequencePlayMode::Forward, 1.0f, false);
}

