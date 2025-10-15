// Fill out your copyright notice in the Description page of Project Settings.


#include "PipouCharacterStateMachine.h"

void UPipouCharacterStateMachine::Init(APipouCharacter* InPipouCharacter)
{
	Character = InPipouCharacter;
}

APipouCharacter* UPipouCharacterStateMachine::GetPipouCharacter() const
{
	return Character;
}
