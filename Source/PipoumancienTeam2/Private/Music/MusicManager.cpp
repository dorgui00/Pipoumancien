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

	if (! IsInWorldStateMusic) return ;
	
	if (IsInCountDown)
	{
	
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
			else if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - Tolerance && !IsAwaitingReply)
			{
				IsAwaitingReply = true;
				continue;
			}
			// check success
			else if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + Tolerance)
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
	}
}


void AMusicManager::InitMusicBySkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
	
	// enable tick
	StartCountDown();
}

F_Note* AMusicManager::GetWaitingNote()
{
	CurrentWaitingNote = &CurrentSkeleton->Notes[CurrentWaitingNoteIndex];
	return CurrentWaitingNote;
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
	Tempo = 0.f;
	IsInCountDown = true;
}
