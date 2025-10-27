// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

AGameManager* AGameManager::MyInstance;

AGameManager* AGameManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<AGameManager>(); 
	}

	return MyInstance;
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::SetWorldMusicState(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
	//change state for players
	// for (auto Character : PipouCharacters)
	// {
	// 	Character->StateMachine->ChangeState(EPipouCharacterStateID::Music);
	// }
	
	//// Pailletas
	// SetAllMusicBehavior()
	// BlockMovement()
	// SetCameraMusic()
	// DisplayUI()
	// MusicManager.InitMusicManager(CurrentS_Skeleton)
}
