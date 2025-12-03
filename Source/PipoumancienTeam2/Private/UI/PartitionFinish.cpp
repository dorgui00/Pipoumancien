// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PartitionFinish.h"
#include "Components/TextBlock.h"

TObjectPtr<UTextBlock> UPartitionFinish::GetWinLoseText()
{
	return WinLoseText;
}

void UPartitionFinish::SetWinLostText(const FString& NewText) const
{
	WinLoseText->SetText(FText::FromString(NewText));
}

void UPartitionFinish::PlayAnimationPartitionFinishStart()
{
	PlayAnimation(PartitionFinishAnimationEnter, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}

void UPartitionFinish::PlayAnimationPartitionFinishExit()
{
	PlayAnimation(PartitionFinishAnimationExit, 0.f, 1, EUMGSequencePlayMode::Forward, 1.f, false);
}
