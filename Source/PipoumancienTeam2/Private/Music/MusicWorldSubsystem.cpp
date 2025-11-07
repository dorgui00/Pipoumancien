// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicWorldSubsystem.h"
#include "InputAction.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "UI/GlobalHUDSubsystem.h"

void UMusicWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UMusicWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Get GlobalGameSubsystem
	GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	IsInWorldStateMusic = false;
}

void UMusicWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!IsInWorldStateMusic) return;
	
	if (IsInCountDown)
	{
		TimerCountDown -= DeltaTime;

		// Finish Countdown
		if (TimerCountDown<=0)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Finish CountDown")), true, FVector2D(2, 2));
			
			IsInCountDown = false;
			TimerCountDown = 3.f;
		}
	}
	else
	{
		Tempo += GetWorld()->GetDeltaSeconds() * Speed;
		
		UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
		if (!HUDSubsystem) return;
		
		HUDSubsystem->MovePartition(DeltaTime);
		
		if (!CurrentSkeleton) return; // Secu check if current skeleton is set

		F_Note* CurrentNote = GetWaitingNote();
		
		// Not yet time for qte => !IsAwaitingReply
		if (Tempo < CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance * Speed)
		{
			IsAwaitingReply = false;
			return;
		}
		
		// Is Awaiting Reply
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance * Speed && !IsAwaitingReply)
		{
			GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Red, FString::Printf(TEXT("INPUT : %s"), *CurrentSkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName()), true, FVector2D(2, 2));
			GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Blue, FString::Printf(TEXT("PITCH : %f"), CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Pitch), true, FVector2D(2, 2));
			
			IsAwaitingReply = true;
			return;
		}
		
		// check success
		if (Tempo >= CurrentSkeleton->Notes[CurrentWaitingNoteIndex].Frequency + (TimeTolerance * Speed))
		{
			IsAwaitingReply = false;

			// success
			if(HasAchievedQte())
			{
				//Melodie finie et réussie
				if (CurrentWaitingNoteIndex == CurrentSkeleton->Notes.Num()-1)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));
			
					//SetActorTickEnabled(false);
					IsInWorldStateMusic = false;
			
					Tempo = 0.f;

					for (auto PipouCharacter : GlobalGameSubsystem->PipouCharacters)
					{
						PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Idle);
					}

					UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();
					CurrentWaitingNoteIndex = 0;
				}
				// go next note
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Go Next Note")), true, FVector2D(2, 2));
				
					Tempo = TimeTolerance;
					
					CurrentWaitingNoteIndex++;
				}
			}
			// lost qte time
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString::Printf(TEXT("Tu as raté la note")), true, FVector2D(2, 2));

				// go back from two previous notes
				CurrentWaitingNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex-2);
				StartCountDown();
			}
			
			// reset
			ResetMusicianReply();
		}
	}
}

void UMusicWorldSubsystem::InitMusic(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
	
	//SetActorTickEnabled(true);
	IsInWorldStateMusic = true;
	
	CurrentCursorValue = 0.f;

	// Spawn Notes in UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->SpawnNotesPartition(CurrentSkeleton);

	StartCountDown();
}

F_Note* UMusicWorldSubsystem::GetWaitingNote() const
{
	if (CurrentWaitingNoteIndex > CurrentSkeleton->Notes.Num()-1)
		UE_LOG(LogTemp, Error, TEXT("Current waiting Note is out of range"));
	
	return &CurrentSkeleton->Notes[CurrentWaitingNoteIndex];
}

void UMusicWorldSubsystem::ResetMusicianReply()
{
	HasMusicianReceivedInput = false;
}

bool UMusicWorldSubsystem::HasAchievedQte() const
{
	if (HasMusicianReceivedInput
		&& GetWaitingNote()->Pitch >= CurrentCursorValue - PitchTolerance
		&& GetWaitingNote()->Pitch <= CurrentCursorValue + PitchTolerance)
	{
		return true;
	}

	return false;
}

void UMusicWorldSubsystem::ReceivedMusicianInput()
{
	if (HasMusicianReceivedInput) return;
	
	HasMusicianReceivedInput = true;
}

void UMusicWorldSubsystem::StartCountDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Black, FString::Printf(TEXT("Start CountDown de 3 sec")), true, FVector2D(2, 2));
	
	Tempo = 0.f;
	IsInCountDown = true;
}
