// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterState.h"

#include "InputActionValue.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
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

void UPipouCharacterState::OnCharacterPressedNote(UInputAction* InputAction)
{
	UGlobalGameSubsystem*  GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	
	if (GlobalGameSubsystem->GetCurrentSkeleton() != nullptr)
	{
		// UE_LOG(LogTemp, Display, TEXT("Add note"));
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Add note")), true, FVector2D{2, 2});
		
		GlobalGameSubsystem->AddNote(InputAction);
	}
}

