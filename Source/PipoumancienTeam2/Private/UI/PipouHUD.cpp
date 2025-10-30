// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PipouHUD.h"
#include "Blueprint/UserWidget.h"
#include "UResurrectionWidget.h"

void APipouHUD::AddWBPResurrection(APlayerController* PlayerController)
{
	if (WBPResurrectionInstance == nullptr) return;
	
	WBPResurrectionInstance = CreateWidget<UResurrectionWidget>(PlayerController, WBPResurrectionClass);
	
	if (WBPResurrectionInstance != nullptr)
	{
		WBPResurrectionInstance->AddToPlayerScreen();
	}
}
