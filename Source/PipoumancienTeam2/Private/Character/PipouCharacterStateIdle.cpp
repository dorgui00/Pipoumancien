// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateIdle.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Game/GameManager.h"

EPipouCharacterStateID UPipouCharacterStateIdle::GetStateID()
{
	return EPipouCharacterStateID::Idle;
}

void UPipouCharacterStateIdle::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetMesh()->PlayAnimation(IdleAnim, true);
		
	Character->InputPressedEvent.AddDynamic(this, &UPipouCharacterStateIdle::OnCharacterPressedInput);
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

	Character->InputPressedEvent.RemoveDynamic(this, &UPipouCharacterStateIdle::OnCharacterPressedInput);
}

void UPipouCharacterStateIdle::OnCharacterPressedInput(UInputAction* InputAction, FInputActionValue InputActionValue)
{
	Super::OnCharacterPressedInput(InputAction, InputActionValue);

}

