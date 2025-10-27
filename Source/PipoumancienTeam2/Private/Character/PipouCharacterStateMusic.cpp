// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"
#include "InputActionValue.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterInputData.h"
#include "Data/F_Note.h"
#include "Game/GameManager.h"
#include "Music/MusicManager.h"

EPipouCharacterStateID UPipouCharacterStateMusic::GetStateID()
{
	return EPipouCharacterStateID::Music;
}

void UPipouCharacterStateMusic::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	InitRoles();
	InitSkeletons();
	InitInputPitch();
	InitMusicManager();
	
	Character->InputPressedEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedInput);
	AMusicManager::Instance()->ResetReplyEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnBeginQTE);
}

void UPipouCharacterStateMusic::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
}

void UPipouCharacterStateMusic::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	Character->InputPressedEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedInput);
	AMusicManager::Instance()->ResetReplyEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnBeginQTE);
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
	Skeleton = AGameManager::Instance()->CurrentSkeleton;
}

void UPipouCharacterStateMusic::InitInputPitch()
{
	InputPitch = Character->InputData->InputPitch;
}

void UPipouCharacterStateMusic::InitMusicManager()
{
	MusicManager = AMusicManager::Instance();
}


// Event Delegates
void UPipouCharacterStateMusic::OnCharacterPressedInput(UInputAction* InputAction, FInputActionValue InputActionValue)
{
	if (CurrentRole == EPipouCharacterRoles::Musician)
	{
		if (MusicManager->IsAwaitingReply && MusicManager->CurrentWaitingNote->InputAction == InputAction)
		{
			HasReceivedInput = true;
		}
	}
	else if (CurrentRole == EPipouCharacterRoles::Conductor)
	{
		if (InputAction == InputPitch)
		{
			MusicManager->CurrentCursorValue += InputActionValue.Get<float>();

			if (MusicManager->IsAwaitingReply &&
				(MusicManager->CurrentWaitingNote->Pitch >= MusicManager->CurrentCursorValue - PitchTolerance ||
				MusicManager->CurrentWaitingNote->Pitch <= MusicManager->CurrentCursorValue + PitchTolerance))
			{
				HasReceivedInput = true;
			}
		}
	}
}

void UPipouCharacterStateMusic::OnBeginQTE()
{
	HasReceivedInput = false;
}

