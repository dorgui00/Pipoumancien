// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Kismet/GameplayStatics.h"
#include "Music/MusicManager.h"


AGameManager* AGameManager::MyInstance ;

AGameManager* AGameManager::Instance()
{
	// if (!MyInstance)
	// {
	// 	MyInstance = NewObject<AGameManager>();
	// }

	return MyInstance;
}

// to call in init pipou chara
void AGameManager::SetCharacters(APipouCharacter* Character)
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
		if (HasValidFirstNotes()) 
			SetWorldMusicState();
		
		ResetInputsArray();
	}
}

bool AGameManager::HasValidFirstNotes()
{
	for (int i = 0; i < NbNotesToCheck; ++i)
	{
		if (CurrentSkeleton->Notes[i].InputAction != InputPressed[i])
		{
			UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes raté"));
			return false;
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes réussi"));
	return true;
}

void AGameManager::ResetInputsArray()
{
	InputPressed.Empty();
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();

	//Init instance in the begin play
	if (!MyInstance)
	{
		MyInstance = Cast<AGameManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameManager::StaticClass()));
	}
}

void AGameManager::SetWorldMusicState()
{

	WorldState = EWorldState::WorldMusic;

	// TO EDIT -- secu pour avoir un TArray<APipouCharacter> à jour (ordre d'execution/initialisation)
	if (PipouCharacters.Num() ==0)
	{
		TArray<AActor*> Characters;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),APipouCharacter::StaticClass(), Characters);
		for (auto Character : Characters)
		{
			APipouCharacter* PipouCharacter = Cast<APipouCharacter>(Character);
			PipouCharacters.Add(PipouCharacter);
		}
	}
	
	//change state for players
	for (auto Character : PipouCharacters) 
	{
		if (Character && Character->StateMachine)
		{
			Character->StateMachine->ChangeState(EPipouCharacterStateID::Music);
		}
	}
	
	//// Pailletas
	// SetAllMusicBehavior()
	// BlockMovement()
	// SetCameraMusic()
	// DisplayUI()
	AMusicManager::Instance()->InitMusicBySkeleton(CurrentSkeleton);
}
