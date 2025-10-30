// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicManager.h"

#include "EngineUtils.h"
#include "InputAction.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GameManager.h"

class UPipouCharacterStateMusic;
AMusicManager* AMusicManager::MyInstance;

AMusicManager::AMusicManager()
{
	PrimaryActorTick.bCanEverTick = true;
}
	
AMusicManager* AMusicManager::Instance(UWorld* World)
{
	if (IsValid(MyInstance))
		return MyInstance;

	for (TActorIterator<AMusicManager> It(World); It; ++It)
	{
		MyInstance = *It;
		break;
	}

	if (!MyInstance)
	{
		MyInstance = World->SpawnActor<AMusicManager>(AMusicManager::StaticClass());
	}

	return MyInstance;
}


void AMusicManager::BeginPlay()
{
	Super::BeginPlay();

	//SetActorTickEnabled(false);
	IsInWorldStateMusic = false;
}


void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Display, TEXT("jveux ticker"));

	if (!IsInWorldStateMusic) return; // TO EDIT
	
	if (IsInCountDown)
	{
		TimerCountDown-=DeltaTime;
		
		if (TimerCountDown<=0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Finish CountDown")));
			// UE_LOG(LogTemp, Display, TEXT("Finish Countdown"));
			
		 	IsInCountDown = false;
			TimerCountDown = 5.f;
		}
	}
	else // 
	{
		Tempo += DeltaTime;

		if (!CurrentSkeleton) return; // secu
		
		if (CurrentWaitingNoteIndex == CurrentSkeleton->Notes.Num()-1)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")));
			// UE_LOG(LogTemp, Display, TEXT("Melodie finie et réussie"));
			
			//SetActorTickEnabled(false);
			IsInWorldStateMusic = false;
			
			Tempo = 0.f;
		}
		
		F_Note CurrentNote = CurrentSkeleton->Notes[CurrentWaitingNoteIndex];

		// Not yet time for qte => !IsAwaitingReply
		if (Tempo < CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency-Tolerance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("Attend l input : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName());
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Blue, TEXT("Attend l input : %f"), CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Pitch);
			
			IsAwaitingReply = false;
			return;
		}
		
		//Is Awaiting Reply
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - Tolerance && !IsAwaitingReply)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, TEXT("PRESS"));
			// UE_LOG(LogTemp, Display, TEXT("Attend l input : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName());
			
			IsAwaitingReply = true;
			return;
		}
		
		// check success
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + Tolerance)
		{
			IsAwaitingReply = false ;
			
			if(HasAchievedQte())
			{
				//go next note
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, TEXT("Go Next Note"));
				
				// UE_LOG(LogTemp, Display, TEXT("Go next note"));
				Tempo = Tolerance;
				CurrentWaitingNoteIndex++;
			}
			else
			{
				StartCountDown();
				CurrentWaitingNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex-2);
			}
			
			ResetReplies();
		}
	}
}


void AMusicManager::InitMusicBySkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
	
	//SetActorTickEnabled(true);
	IsInWorldStateMusic = true;
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
	HasMusicianReceivedInput = false;
}

bool AMusicManager::HasAchievedQte()
{
	if (HasMusicianReceivedInput && (GetWaitingNote()->Pitch >= CurrentCursorValue - PitchTolerance ||
			GetWaitingNote()->Pitch <= CurrentCursorValue + PitchTolerance))
	{
		return true;
	}

	return false;
}

void AMusicManager::CheckReceivedInput()
{
	if (!HasMusicianReceivedInput)
	{
		HasMusicianReceivedInput = true;
	}
}

void AMusicManager::StartCountDown()
{
	// UE_LOG(LogTemp, Display, TEXT("Start Count Down de 3sec"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, TEXT("Start CountDown de 3 sec"));
	
	Tempo = 0.f;
	IsInCountDown = true;
}
