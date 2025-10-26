// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterState.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"

UPipouCharacterState::UPipouCharacterState()
{
	PrimaryComponentTick.bCanEverTick = false;
}

EPipouCharacterStateID UPipouCharacterState::GetStateID()
{
	return EPipouCharacterStateID::None;
}

void UPipouCharacterState::StateInit(UPipouCharacterStateMachine* InStateMachine)
{
	StateMachine = InStateMachine;
	Character = InStateMachine->GetPipouCharacter();
}

void UPipouCharacterState::StateEnter(EPipouCharacterStateID PreviousStateID)
{
}

void UPipouCharacterState::StateTick(float Deltatime)
{
}
	
void UPipouCharacterState::StateExit(EPipouCharacterStateID NextStateID)
{
}
