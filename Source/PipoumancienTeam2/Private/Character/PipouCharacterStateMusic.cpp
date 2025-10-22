// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"

#include "Character/PipouCharacter.h"


EPipouCharacterStateID UPipouCharacterStateMusic::GetStateID()
{
	return EPipouCharacterStateID::Music;
}

void UPipouCharacterStateMusic::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	//Character->GetPipouCharacterClass()
}

void UPipouCharacterStateMusic::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);

	//chef d'orchestre
	if (Character->GetPipouCharacterClass()==EPipouCharacterClass::Conductor)
	{
		if (Character->GetInputNoteA())
		{
			printf("A orchestre");
		}
	}
	// musician
	else
	{
		if (Character->GetInputPitch())
		{
			printf("Pitch etre un enfant c'est du sport ");
		}
	}
}

void UPipouCharacterStateMusic::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}
