// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/PipouCharacterStateWalk.h"

#include "Camera/CameraWorldSubsystem.h" // ADDED
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"

EPipouCharacterStateID UPipouCharacterStateWalk::GetStateID()
{
	return EPipouCharacterStateID::Walk;
}

void UPipouCharacterStateWalk::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetMesh()->PlayAnimation(WalkAnim, true);

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
	
	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Red,
	// 	TEXT("Enter StateWalk")
	// );
}

void UPipouCharacterStateWalk::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);

	if (Character->GetInputMoveXY().SquaredLength() <= Character->DeadZone * Character->DeadZone)
	{
		StateMachine->ChangeState(EPipouCharacterStateID::Idle);
	}
	else
	{
		FVector MoveDir = Character->CameraActor->GetActorForwardVector() * FMath::Sign(Character->GetInputMoveXY().Y);
		MoveDir += Character->CameraActor->GetActorRightVector() * FMath::Sign(Character->GetInputMoveXY().X);
		MoveDir.Normalize();
		Character->SetOrientXY(FVector2D(MoveDir.X, MoveDir.Y));
		Character->AddMovementInput(MoveDir, 1);
		
		//ADDED
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
	}

}

void UPipouCharacterStateWalk::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	// GEngine->AddOnScreenDebugMessage(
	// 	-1,
	// 	3.f,
	// 	FColor::Red,
	// 	TEXT("Exit StateWalk")
	// );
}
