// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"

#include "InputActionValue.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterInputData.h"

EPipouCharacterStateID UPipouCharacterStateMusic::GetStateID()
{
	return EPipouCharacterStateID::Music;
}

void UPipouCharacterStateMusic::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	InitRoles();
	InitSkeletons();
	InputPitch = Character->InputData->InputPitch;
	Character->InputPressedEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedInput);
}

void UPipouCharacterStateMusic::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
}

void UPipouCharacterStateMusic::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	Character->InputPressedEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedInput);
}

// Music
void UPipouCharacterStateMusic::InitRoles()
{
	switch (Character->GetPipouCharacterClass())
	{
	case EPipouCharacterClass::Necro:
		CurrentRole = EPipouCharacterRoles::Musician;
		
	case EPipouCharacterClass::Phantom:
		CurrentRole = EPipouCharacterRoles::Conductor;
		
	default:
		CurrentRole = EPipouCharacterRoles::None;
	}
}

void UPipouCharacterStateMusic::InitSkeletons()
{
}

void UPipouCharacterStateMusic::OnCharacterPressedInput(UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if (CurrentRole == EPipouCharacterRoles::Musician)
	{
		
	}
	else if (CurrentRole == EPipouCharacterRoles::Conductor)
	{
		
	}
}

