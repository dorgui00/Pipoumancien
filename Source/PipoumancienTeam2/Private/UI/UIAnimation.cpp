// Fill out your copyright notice in the Description page of Project Settings.

#include "BP/BP_ZoneSquelette.h"
#include "UI/UIAnimation.h"




void UUIAnimation::NativeConstruct()
{
	PlayAnimation(AnimationHelp, 0.0f, 3, EUMGSequencePlayMode::Forward, 1.0f, false);
}

void UUIAnimation::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
	if (!IsAnimationPlaying(AnimationHelp))
	{
		UE_LOG(LogTemp, Warning, TEXT("AnimationHelp n'est PAS en lecture."));
	}
}



