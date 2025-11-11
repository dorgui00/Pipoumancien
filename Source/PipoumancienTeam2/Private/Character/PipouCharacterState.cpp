// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterState.h"

#include "InputActionValue.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Game/GlobalGameSubsystem.h"
#include "Interaction/Interact.h"
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
		IsTryingToInteractWithWorld = false;
		CurrentWorldNotes = 0;

		//Interact with interactor
		if (Character->Interactor)
		{
			IInteract::Execute_Interact(Character->Interactor.GetObject());
		}
	}
	// Waiting for another note
	else
	{
		IsTryingToInteractWithWorld = true;
	}
	
	WorldNotesTimer = 0.f; // reset timer 
}

void UPipouCharacterState::OnCharacterPressedNote(UInputAction* InputAction)
{
	UGlobalGameSubsystem*  GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();

	// si mon interactor est de type current skeleton
	if (GlobalGameSubsystem->GetCurrentSkeleton() != nullptr)
	{
		// UE_LOG(LogTemp, Display, TEXT("Add note"));
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Add note")), true, FVector2D{2, 2});
		
		GlobalGameSubsystem->AddNoteForSkeletonInteraction(InputAction);
	}
	//sinon si j ai un interactor : World Interaction
	if (Character->Interactor)
	{
		AddNoteForWorldInteraction();
	}
	
}

