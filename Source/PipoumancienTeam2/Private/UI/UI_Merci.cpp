// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Merci.h"

#include "Components/TextBlock.h"
#include "Data/F_Skeleton.h"

void UUIMerci::SetMerci(F_Skeleton* Skeleton)
{
	CurrentDialogue = Skeleton->Thanks;
}

void UUIMerci::NativeConstruct()
{
	Super::NativeConstruct();

	Trenks->SetText(FText::FromString(CurrentDialogue));
}
