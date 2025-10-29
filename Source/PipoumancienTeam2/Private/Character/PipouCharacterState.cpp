// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterState.h"

#include "InputActionValue.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Game/GameManager.h"

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



void UPipouCharacterState::OnCharacterPressedInput(UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if (AGameManager::Instance()->GetCurrentSkeleton() != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Add note"));
		AGameManager::Instance()->AddNote(InputAction);
	}
}

