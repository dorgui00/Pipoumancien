// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateDialogue.h"


EPipouCharacterStateID UPipouCharacterStateDialogue::GetStateID()
{
	return EPipouCharacterStateID::Dialogue;
}

void UPipouCharacterStateDialogue::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
}

void UPipouCharacterStateDialogue::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
}

void UPipouCharacterStateDialogue::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
}
