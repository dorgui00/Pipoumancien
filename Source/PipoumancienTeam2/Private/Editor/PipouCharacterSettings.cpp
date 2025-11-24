// Fill out your copyright notice in the Description page of Project Settings.


#include "Editor/PipouCharacterSettings.h"

void UPipouCharacterSettings::InitWorldSounds()
{
	WorldSoundFromInput = {
		{ InputData->InputNoteY, UpSound.LoadSynchronous()},
		{ InputData->InputNoteB, RightSound.LoadSynchronous() },
		{ InputData->InputNoteA, DownSound.LoadSynchronous() }, 
		{ InputData->InputNoteX, LeftSound.LoadSynchronous() }
	};
}

TObjectPtr<USoundBase> UPipouCharacterSettings::GetWorldSoundFromInput(TObjectPtr<UInputAction> InputAction) const
{
	return WorldSoundFromInput[InputAction];
}
