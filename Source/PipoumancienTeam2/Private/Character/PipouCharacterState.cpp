// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterState.h"

#include "InputActionValue.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Game/GlobalGameSubsystem.h"
#include "Interaction/Interact.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"

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

void UPipouCharacterState::OnCharacterPitch(FInputActionValue InputActionValue)
{
	
}

void UPipouCharacterState::AddNoteForWorldInteraction()
{
	CurrentWorldNotes ++;

	// Succeeded World Interaction
	if (CurrentWorldNotes>=WorldNotesToPlay)
	{
		
		ResetWorldInteraction();
		
		//Interact with interactor
		if (Character->Interactor)
		{
			IInteract::Execute_Interact(Character->Interactor.GetObject());
		}
	}
	// Waiting for another note
	else
	{
		WorldNotesTimer = 0.f; // reset timer
		
		IsTryingToInteractWithWorld = true;
	}
}

// call at the end of the overlap (cancel interaction)
//		|| at the end of world interaction (interaction finished)
void UPipouCharacterState::ResetWorldInteraction()
{
	IsTryingToInteractWithWorld = false;
	CurrentWorldNotes = 0;

	WorldNotesTimer = 0;
}

void UPipouCharacterState::OnCharacterPressedNote(UInputAction* InputAction)
{
	TObjectPtr<UGlobalGameSubsystem> GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	if (!GlobalGameSubsystem) return;
	
	// if players overlap the same skeleton
	if (GlobalGameSubsystem->GetCurrentSkeleton())
	{
		// UE_LOG(LogTemp, Display, TEXT("Add note"));
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Add note")), true, FVector2D{2, 2});
		
		GlobalGameSubsystem->AddNoteForSkeletonInteraction(InputAction);
	}
	// else if I have an interactor : World Interaction
	// else if => can't play one music to trigger skeleton & world at the same time
	// if => can trigger World && Skeleton at the same time
	else if (Character->Interactor)
	{
		AddNoteForWorldInteraction();
	}
}

void UPipouCharacterState::OnCharacterTriggeredNote(UInputAction* InputAction)
{
	// SAME AS THE PRESSED NOTE EVENT BUT WITH SECURITY TO AVOID MULTIPLE PRESSED AT THE SAME TIME

	TObjectPtr<UGlobalGameSubsystem> GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	if (!GlobalGameSubsystem) return;
	
	// if players overlap the same skeleton
	if (GlobalGameSubsystem->GetCurrentSkeleton() && !HasPressedNotes)
	{
		HasPressedNotes = true;

		// UE_LOG(LogTemp, Display, TEXT("Add note"));
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Add note")), true, FVector2D{2, 2});
		
		GlobalGameSubsystem->AddNoteForSkeletonInteraction(InputAction);
	}
	// else if I have an interactor : World Interaction
	// else if => can't play one music to trigger skeleton & world at the same time
	// if => can trigger World && Skeleton at the same time
	else if (Character->Interactor && !HasPressedNotes)
	{
		HasPressedNotes = true;
		AddNoteForWorldInteraction();
	}
}

