// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/PipouCharacterStateMachine.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterState.h"
#include "Character/PipouCharacterStateID.h"

void UPipouCharacterStateMachine::Init(APipouCharacter* InPipouCharacter)
{
	Character = InPipouCharacter;
	FindStates();
	InitStates();

	ChangeState(EPipouCharacterStateID::Idle);
}

void UPipouCharacterStateMachine::Tick(float DeltaTime)
{
	if (CurrentState == nullptr) return;
	CurrentState->StateTick(DeltaTime);
}

APipouCharacter* UPipouCharacterStateMachine::GetPipouCharacter() const
{
	return Character;
}

void UPipouCharacterStateMachine::FindStates()
{
	TArray<UActorComponent*> FoundComponents = Character->K2_GetComponentsByClass(UPipouCharacterState::StaticClass());
	for (UActorComponent* StateComponent : FoundComponents)
	{
		UPipouCharacterState* State = Cast<UPipouCharacterState>(StateComponent);
		if (State == nullptr) continue;
		if (State->GetStateID() == EPipouCharacterStateID::None) continue;

		AllStates.Add(State);
	}
}

void UPipouCharacterStateMachine::InitStates()
{
	for (UPipouCharacterState* State : AllStates)
	{
		State->StateInit(this);
	}
}

UPipouCharacterState* UPipouCharacterStateMachine::GetState(EPipouCharacterStateID StateID)
{
	for (UPipouCharacterState* State : AllStates)
	{
		if (State->GetStateID() == StateID) return State;
	}

	return nullptr;
}

// TO SUPP ?
UPipouCharacterState* UPipouCharacterStateMachine::GetCurrentState()
{
	return CurrentState;
}

void UPipouCharacterStateMachine::ChangeState(EPipouCharacterStateID NextStateID)
{
	UPipouCharacterState* NextState = GetState(NextStateID);
	if (NextState == nullptr) return;

	if (CurrentState != nullptr)
	{
		CurrentState->StateExit(NextStateID); 
	}

	EPipouCharacterStateID PreviousStateID = CurrentStateID;
	CurrentStateID = NextStateID;
	CurrentState = NextState;

	if (CurrentState != nullptr)
	{
		CurrentState->StateEnter(PreviousStateID);
	}
}

