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
		
	Character->InputPressedNoteEvent.AddDynamic(this, &UPipouCharacterStateIdle::OnCharacterPressedNote);
}

void UPipouCharacterStateIdle::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);

	if (Character->GetInputMoveXY().SquaredLength() > Character->DeadZone * Character->DeadZone)
	{
		StateMachine->ChangeState(EPipouCharacterStateID::Walk);
	}

	// Waiting for world interaction or not
	if (IsTryingToInteractWithWorld)
	{
		WorldNotesTimer += Deltatime;
		
		//time ended
		if (WorldNotesTimer >= WorldNotesInterval)
		{
			WorldNotesTimer = 0.f;
			IsTryingToInteractWithWorld = false;
		}
	}
}

void UPipouCharacterStateIdle::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->InputPressedNoteEvent.RemoveDynamic(this, &UPipouCharacterStateIdle::OnCharacterPressedNote);
}

void UPipouCharacterStateIdle::OnCharacterPressedNote(UInputAction* InputAction)
{
	Super::OnCharacterPressedNote(InputAction);
}

