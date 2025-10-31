// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicManager.h"

#include "EngineUtils.h"
#include "InputAction.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
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
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Finish CountDown")), true, FVector2D(2, 2));
			// UE_LOG(LogTemp, Display, TEXT("Finish Countdown"));
			
		 	IsInCountDown = false;
			TimerCountDown = 3.f;
		}
	}
	else // 
	{
		Tempo += DeltaTime;

		if (!CurrentSkeleton) return; // secu

		
		if (CurrentWaitingNoteIndex == CurrentSkeleton->Notes.Num())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));
			// UE_LOG(LogTemp, Display, TEXT("Melodie finie et réussie"));
			
			//SetActorTickEnabled(false);
			IsInWorldStateMusic = false;
			
			Tempo = 0.f;

			if (AGameManager::Instance(GetWorld()) != nullptr)
			{
				for (auto PipouCharacter : AGameManager::Instance(GetWorld())->PipouCharacters)
				{
					PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Idle);
				}

				AGameManager::Instance(GetWorld())->RemoveResurrectionUI();
			}

			

			return;
		}

		F_Note CurrentNote = CurrentSkeleton->Notes[CurrentWaitingNoteIndex];
		
		// Not yet time for qte => !IsAwaitingReply
		if (Tempo < CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency-Tolerance)
		{
			// if (!HasPrint)
			// {
			// 	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Attend l input : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName()));
			// 	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Attend l input : %f"), CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Pitch));
			//
			// 	HasPrint = true;
			// }
			
			IsAwaitingReply = false;
			return;
		}
		
		//Is Awaiting Reply
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - Tolerance && !IsAwaitingReply)
		{
			GEngine->AddOnScreenDebugMessage(-1, Tolerance * 2, FColor::Red, FString::Printf(TEXT("INPUT : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName()), true, FVector2D(2, 2));
			GEngine->AddOnScreenDebugMessage(-1, Tolerance * 2, FColor::Blue, FString::Printf(TEXT("PITCH : %f"), CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Pitch), true, FVector2D(2, 2));
			// UE_LOG(LogTemp, Display, TEXT("Attend l input : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName());
			
			IsAwaitingReply = true;
			return;
		}
		
		// check success
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + Tolerance)
		{
			IsAwaitingReply = false;
			
			if(HasAchievedQte())
			{
				//go next note
				GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Go Next Note")), true, FVector2D(2, 2));
				
				// UE_LOG(LogTemp, Display, TEXT("Go next note"));
				Tempo = Tolerance;
				CurrentWaitingNoteIndex++;
			}
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString::Printf(TEXT("Tu as raté  la note")), true, FVector2D(2, 2));
				CurrentWaitingNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex-2);
				StartCountDown();
			}

			HasPrint = false;
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
	if (HasMusicianReceivedInput && (GetWaitingNote()->Pitch >= CurrentCursorValue - PitchTolerance &&
			GetWaitingNote()->Pitch <= CurrentCursorValue + PitchTolerance))
	{
		return true;
	}

	return false;
}

void AMusicManager::CheckReceivedInput()
{
	if (HasMusicianReceivedInput) return;
	HasMusicianReceivedInput = true;
}

void AMusicManager::StartCountDown()
{
	// UE_LOG(LogTemp, Display, TEXT("Start Count Down de 3sec"));
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Start CountDown de 3 sec")), true, FVector2D(2, 2));
	
	Tempo = 0.f;
	IsInCountDown = true;
}
