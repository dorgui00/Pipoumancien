// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicManager.h"

#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"

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

			if (Tempo < CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency-Tolerance)
			{
				IsAwaitingReply = false;
				continue;
			}
			else if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - Tolerance && !IsAwaitingReply)
			{
				IsAwaitingReply = true;
				continue;
			}
			else if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + Tolerance)
			{
				// au max du tps check si il a réussit
				IsAwaitingReply = false ;
				ResetReplyEvent.Broadcast();
				// if (!HasReceivedReply player0 || !HasReceivedReply player1) // loose
				// {
				// 	StartCountDown();
				// 	CurrentWaitingNoteIndex -= 2;
				// 	break;
				// }
				// else
				// {
				// 	//go next note
				// 	Tempo = Tolerance;
				// }
			}
			
		}
	}
}

void AMusicManager::InitMusicManager(F_Skeleton* Skeleton)
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

void AMusicManager::StartCountDown()
{
	IsInCountDown = true;
	Tempo = 0.f;
}
