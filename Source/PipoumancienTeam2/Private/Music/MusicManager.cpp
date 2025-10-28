// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicManager.h"

#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GameManager.h"

class UPipouCharacterStateMusic;
AMusicManager* AMusicManager::MyInstance;
	
AMusicManager* AMusicManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<AMusicManager>(); 
	}

	return MyInstance;
}

void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// isInWorldState
	
	if (IsInCountDown)
	{
		TimerCountDown-=DeltaTime;
		
		if (TimerCountDown<=0)
		{
			UE_LOG(LogTemp, Display, TEXT("Finish Countdown"));
			
		 	IsInCountDown = false;
			TimerCountDown = 3.f;
		}
	}
	else // 
	{
		Tempo += DeltaTime;
		
		for (int i = CurrentWaitingNoteIndex; i < CurrentSkeleton->Notes.Num(); i++)
		{
			CurrentWaitingNoteIndex = i;

			// Not yet time for qte => !IsAwaitingReply
			if (Tempo < CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency-Tolerance)
			{
				IsAwaitingReply = false;
				continue;
			}
			//Is Awaiting Reply
			if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - Tolerance && !IsAwaitingReply)
			{
				//UE_LOG(LogTemp, Display, TEXT("Attend l input %s", CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction.ToString()));
				
				IsAwaitingReply = true;
				continue;
			}
			// check success
			if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + Tolerance)
			{
				IsAwaitingReply = false ;
				ResetReplies();
				
				if(HasAchievedQte())
				{
					//go next note
                	Tempo = Tolerance;
				}
				else
				{
					StartCountDown();
					CurrentWaitingNoteIndex -= 2;
					break; // out loop to go inCountdown
				}
			}
		}
		
		UE_LOG(LogTemp, Display, TEXT("Melodie finie et réussie"));
		SetActorTickEnabled(false);
	}
}


void AMusicManager::InitMusicBySkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
	
	SetActorTickEnabled(true);
	StartCountDown();
}

F_Note* AMusicManager::GetWaitingNote()
{
	return &CurrentSkeleton->Notes[CurrentWaitingNoteIndex];
}

void AMusicManager::ReceiveInput()
{
	Replies++;	
}

void AMusicManager::ResetReplies()
{
	Replies = 0;
}

bool AMusicManager::HasAchievedQte()
{
	AGameManager* GameManager = AGameManager::Instance();
	
	return Replies >= GameManager->PipouCharacters.Num();
}

void AMusicManager::StartCountDown()
{
	UE_LOG(LogTemp, Display, TEXT("Start Count Down de 3sec"));
	
	Tempo = 0.f;
	IsInCountDown = true;
}
