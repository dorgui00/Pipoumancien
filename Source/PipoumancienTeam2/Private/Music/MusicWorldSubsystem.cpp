// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicWorldSubsystem.h"
#include "InputAction.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "PNJ/SkeletonController.h"
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

	GlobalHUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
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
		Tempo += DeltaTime * Speed;
		
		UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
		if (!HUDSubsystem) return;
		
		HUDSubsystem->MovePartition(DeltaTime);
		
		if (!CurrentSkeleton) return; // Secu check if current skeleton is set

		F_Note* CurrentNote = GetWaitingNote();
		USlot* CurrentNoteSlot = HUDSubsystem->NotesInstanciated[CurrentWaitingNoteIndex];

		if (CurrentWaitingNoteIndex > 0)
		{
			HUDSubsystem->UiOffset = 0.f;
		}
		
		// Not yet time for qte => !IsAwaitingReply
		if (Tempo < ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * Speed))
		{
			IsAwaitingReply = false;
			CurrentNoteSlot->NoteImage->SetColorAndOpacity({1, 0, 0, 1.f});
			
			return;
		}
		
		// Is Awaiting Reply
		if (Tempo >= ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * Speed) && !IsAwaitingReply)
		{
			GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Red, FString::Printf(TEXT("INPUT : %s"), *CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName()), true, FVector2D(2, 2));
			GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Blue, FString::Printf(TEXT("PITCH : %f"), CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Pitch), true, FVector2D(2, 2));
			CurrentNoteSlot->NoteImage->SetColorAndOpacity({0, 1, 0, 1.f});
			
			IsAwaitingReply = true;
			return;
		}
		
		// check success
		if (Tempo >= ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency + TimeTolerance) * Speed))
		{
			IsAwaitingReply = false;

			// success
			if(HasAchievedQte())
			{
				CurrentNoteSlot->RemoveFromParent();
				
				//Melodie finie et réussie
				if (CurrentWaitingNoteIndex == CurrentSkeleton->MySkeleton->Notes.Num()-1)
				{
					EndMelody();
				}
				// go next note
				else
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Go Next Note")), true, FVector2D(2, 2));
					
					Tempo = TimeTolerance * Speed;
					CurrentWaitingNoteIndex++;
				}
			}
			// lost qte time
			else
			{
				GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Black, FString::Printf(TEXT("Tu as raté la note")), true, FVector2D(2, 2));
				CurrentNoteSlot->NoteImage->SetColorAndOpacity({1, 0, 0, 1.f});
				
				// go back from two previous notes
				CurrentWaitingNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex-2);
				StartCountDown();
			}
			
			// reset
			ResetMusicianReply();
		}
	}
}

void UMusicWorldSubsystem::InitMusic(ASkeletonController* Skeleton)
{
	// Init Data
	CurrentSkeleton = Skeleton;

	// Init InValues
	CurrentCursorValue = 0.f;

	// Spawn Notes in UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->SpawnNotesPartition(CurrentSkeleton);
	
	IsInWorldStateMusic = true;
	
	StartCountDown();
}

void UMusicWorldSubsystem::EndMelody()
{
	// DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));

	// Reset Music
	IsInWorldStateMusic = false;
	Tempo = 0.f;
	CurrentWaitingNoteIndex = 0;

	// Pass to transport
	for (auto PipouCharacter : GlobalGameSubsystem->PipouCharacters)
	{
		PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Idle);
	}

	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();
}

F_Note* UMusicWorldSubsystem::GetWaitingNote() const
{
	if (CurrentWaitingNoteIndex > CurrentSkeleton->MySkeleton->Notes.Num()-1)
		UE_LOG(LogTemp, Error, TEXT("Current waiting Note is out of range"));
	
	return &CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex];
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
