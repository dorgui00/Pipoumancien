// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"
#include "InputActionValue.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterInputData.h"
#include "Data/F_Note.h"
#include "Game/GameManager.h"
#include "Music/MusicManager.h"
#include "UI/UResurrectionWidget.h"

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
	SetMusicManager();

	// UE_LOG(LogTemp, Display, TEXT("Entre dans le state music"));

	Character->InputPressedNoteEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputPitchEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitch);
}

void UPipouCharacterStateMusic::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
}

void UPipouCharacterStateMusic::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->InputPressedNoteEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputPitchEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitch);
}

// Music
void UPipouCharacterStateMusic::InitRoles()
{
	switch (Character->GetPipouCharacterClass())
	{
	case EPipouCharacterClass::Necro:
		CurrentRole = EPipouCharacterRoles::Musician;
		break;
		
	case EPipouCharacterClass::Phantom:
		CurrentRole = EPipouCharacterRoles::Conductor;
		break;
		
	default:
		CurrentRole = EPipouCharacterRoles::None;
	}
}

void UPipouCharacterStateMusic::InitSkeletons()
{
	Skeleton = AGameManager::Instance(GetWorld())->GetCurrentSkeleton();
}

void UPipouCharacterStateMusic::InitInputPitch()
{
	InputPitch = Character->InputData->InputPitch;
}

void UPipouCharacterStateMusic::SetMusicManager()
{
	MusicManager = AMusicManager::Instance(GetWorld());
}


// Event Delegates
void UPipouCharacterStateMusic::OnCharacterPitch(FInputActionValue InputActionValue)
{
	if (CurrentRole == EPipouCharacterRoles::Conductor)
	{
		if (InputActionValue.Get<float>() >= -0.1f && InputActionValue.Get<float>() <= 0.1f) return;
		
		MusicManager->CurrentCursorValue = FMath::Clamp(MusicManager->CurrentCursorValue + InputActionValue.Get<float>() * 0.1f,
			-1.f, 1.0f);

		if (Character->GetHUD() != nullptr)
		{
			Character->GetHUD()->WBPResurrectionInstance->SetSliderPitch(MusicManager->CurrentCursorValue);
		}
		
		// UE_LOG(LogTemp, Display, TEXT("CurrentCursorValue: %f"), MusicManager->CurrentCursorValue);
		//
		// if (MusicManager->IsAwaitingReply &&
		// 	(MusicManager->GetWaitingNote()->Pitch >= MusicManager->CurrentCursorValue - PitchTolerance ||
		// 	MusicManager->GetWaitingNote()->Pitch <= MusicManager->CurrentCursorValue + PitchTolerance))
		// {
		// 	MusicManager->ReceiveInput();
		// }
	}
}

void UPipouCharacterStateMusic::OnCharacterPressedNote(UInputAction* InputAction)
{
	if (CurrentRole == EPipouCharacterRoles::Musician)
	{
		if (MusicManager->IsAwaitingReply && MusicManager->GetWaitingNote()->InputAction == InputAction)
		{
			MusicManager->CheckReceivedInput();
		}
	}
}



