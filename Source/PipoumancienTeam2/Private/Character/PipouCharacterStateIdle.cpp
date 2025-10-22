// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateIdle.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"

EPipouCharacterStateID UPipouCharacterStateIdle::GetStateID()
{
	return EPipouCharacterStateID::Idle;
}

void UPipouCharacterStateIdle::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetMesh()->PlayAnimation(IdleAnim, true);
		
	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Enter StateIdle")
	);
}

void UPipouCharacterStateIdle::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);

	if (Character->GetInputMoveXY().SquaredLength() > Character->DeadZone * Character->DeadZone)
	{
		StateMachine->ChangeState(EPipouCharacterStateID::Walk);
	}
}

void UPipouCharacterStateIdle::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	GEngine->AddOnScreenDebugMessage(
		-1,
		3.f,
		FColor::Red,
		TEXT("Exit StateIdle")
	);
}
