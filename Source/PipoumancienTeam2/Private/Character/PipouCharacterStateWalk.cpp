// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/PipouCharacterStateWalk.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

// ---- STATE DEFAULT FUNCTIONS ----
EPipouCharacterStateID UPipouCharacterStateWalk::GetStateID()
{
	return EPipouCharacterStateID::Walk;
}

void UPipouCharacterStateWalk::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetMesh()->PlayAnimation(WalkAnim, true);

	Character->InputPressedNoteEvent.AddDynamic(this, &UPipouCharacterStateWalk::OnCharacterPressedNote);
}

void UPipouCharacterStateWalk::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
	
	if (UCameraWorldSubsystem* CamSys = GetWorld()->GetSubsystem<UCameraWorldSubsystem>())
	{
		FVector ClampedPos;
		
		bool bInside = CamSys->ClampPositionInsideQuad(Character->GetActorLocation(), ClampedPos);
	
		// clamp position if outside
		if (!bInside)
		{
			Character->SetActorLocation(ClampedPos);
		}
	}

	if (Character->GetInputMoveXY().SquaredLength() <= Character->DeadZone * Character->DeadZone)
	{
		StateMachine->ChangeState(EPipouCharacterStateID::Idle);
	}
	else
	{
		MoveDir = Character->CameraMain->GetForwardVector() * FMath::Sign(Character->GetInputMoveXY().Y);
		MoveDir += Character->CameraMain->GetRightVector() * FMath::Sign(Character->GetInputMoveXY().X);
		MoveDir.Normalize();
		Character->SetOrientXY(FVector2D(MoveDir.X, MoveDir.Y));
		FVector NextPos = Character->GetActorLocation() +  MoveDir * MoveSpeed;
		
		// Camera 
		if (UCameraWorldSubsystem* CamSys = GetWorld()->GetSubsystem<UCameraWorldSubsystem>())
		{
			FVector ClampedPos;
			bool bInside = CamSys->ClampPositionInsideQuad(NextPos, ClampedPos);
			
			// clamp position if outside
			if (!bInside)
			{
				Character->SetActorLocation(ClampedPos);
			}
			else
			{
				Character->SetActorLocation(NextPos);
			}
		}
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

void UPipouCharacterStateWalk::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	
	Character->InputPressedNoteEvent.RemoveDynamic(this, &UPipouCharacterStateWalk::OnCharacterPressedNote);
}

// ---- MOVEMENTS ----
FVector UPipouCharacterStateWalk::GetMoveDirection() const
{
	return MoveDir;
}

void UPipouCharacterStateWalk::OnCharacterPressedNote(UInputAction* InputAction)
{
	// For now can't Interact in state walk
	//Super::OnCharacterPressedNote(InputAction);

	//Character->GetMesh()->PlayAnimation(MusicWalkAnim,false);
}
