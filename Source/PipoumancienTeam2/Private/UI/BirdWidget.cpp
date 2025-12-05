// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/BirdWidget.h"

void UBirdWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UE_LOG(LogTemp, Warning, TEXT("Image01 = %s"), Image01 ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Image02 = %s"), Image02 ? TEXT("OK") : TEXT("NULL"));
	UE_LOG(LogTemp, Warning, TEXT("Image03 = %s"), Image03 ? TEXT("OK") : TEXT("NULL"));
	Images = {Image01,Image02,Image03};
	
}
