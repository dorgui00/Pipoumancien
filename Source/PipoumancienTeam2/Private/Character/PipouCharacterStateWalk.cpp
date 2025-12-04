// Fill out your copyright notice in the Description page of Project Settings.



#include "Character/PipouCharacterStateWalk.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"


// ---- STATE DEFAULT FUNCTIONS ----
EPipouCharacterStateID UPipouCharacterStateWalk::GetStateID()
{
	return EPipouCharacterStateID::Walk;
}

void UPipouCharacterStateWalk::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	Character->GetMesh()->PlayAnimation(WalkAnim, true);
	// -------------- AUDIO -----------------
	//walk steps audio start
	if (WalkLoopSoundNecro && WalkLoopSoundDog)
	{
		if (AActor* Owner = GetOwner())
		{
			if (Owner->GetName() == TEXT("BP_NecroCharacter0")) //verify if its necromancer or dog
			{
				WalkLoopComponent = UGameplayStatics::SpawnSoundAttached(
					WalkLoopSoundNecro,
					Character->GetRootComponent());

			}
			else {

				WalkLoopComponent = UGameplayStatics::SpawnSoundAttached(
					WalkLoopSoundDog,
					Character->GetRootComponent());
			}
		}
	}

	Character->GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;
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
		FVector NextPos = Character->GetActorLocation() + (MoveDir * MoveSpeed * Deltatime);
		
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
				Character->AddMovementInput(MoveDir);
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
	//walk steps audio stop
	if (WalkLoopComponent)
	{
		WalkLoopComponent->FadeOut(0.5f, 0.f);
		WalkLoopComponent = nullptr;
	}

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
