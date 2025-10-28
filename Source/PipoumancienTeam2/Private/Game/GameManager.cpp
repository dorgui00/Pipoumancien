// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"


AGameManager* AGameManager::MyInstance;

AGameManager* AGameManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<AGameManager>(); 
	}

	return MyInstance;
}

// to call in init pipou chara
void AGameManager::SetPipouCharacter(APipouCharacter* Character)
{
	PipouCharacters.Add(Character);
}

F_Skeleton* AGameManager::GetCurrentSkeleton()
{
	return CurrentSkeleton;
}

void AGameManager::SetCurrentSkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
}

// Music
void AGameManager::AddNote(UInputAction* InputAction)
{
	InputPressed.Add(InputAction);

	if (InputPressed.Num() >= NbNotesToCheck)
	{
		ChechThreeFirstNote();
	}
}

void AGameManager::ChechThreeFirstNote()
{
	for (int i = 0; i < NbNotesToCheck; ++i)
	{
		if (CurrentSkeleton->Notes[i].InputAction != InputPressed[i])
		{
			return;
		}
	}

	SetWorldMusicState();
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::SetWorldMusicState()
{
	//change state for players
	for (auto Character : PipouCharacters) 
	{
		Character->StateMachine->ChangeState(EPipouCharacterStateID::Music);
	} 
	
	//// Pailletas
	// SetAllMusicBehavior()
	// BlockMovement()
	// SetCameraMusic()
	// DisplayUI()
	// MusicManager.InitMusicManager(CurrentS_Skeleton)
}
