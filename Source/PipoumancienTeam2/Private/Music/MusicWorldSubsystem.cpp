// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicWorldSubsystem.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Components/Slider.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "UI/GlobalHUDSubsystem.h"
#include "UI/UResurrectionWidget.h"

#pragma region MusicWorldSubsystem
void UMusicWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UMusicWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	// Get GlobalGameSubsystem.
	GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	IsInWorldStateMusic = false;

	// Get GlobalHUDSubsystem.
	GlobalHUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
	GlobalHUDSubsystem->MusicWorldSubsystem = this;

	// Init Subsystem Settings.
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;

	// Initialize Global Music Speed.
	MusicGlobalSpeed = SubsystemSettings->MusicGlobalSpeed;

	// Initialize TimeTolerance.
	TimeTolerance = SubsystemSettings->TimeTolerance;

	// Initialize Pitch Tolerance.
	PitchTolerance = SubsystemSettings->PitchTolerance;

	// Initialize MaxFailNotePossible.
	MaxFailNotePossible = SubsystemSettings->MaxFailNotePossible;
}

#pragma endregion

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
		if (!GlobalHUDSubsystem) return;

		GlobalHUDSubsystem->MovePartition(DeltaTime);
		
		if (IsLerpingOffset)
		{
			TimerLerpingOffset += DeltaTime * MusicGlobalSpeed;
			
			if (TimerLerpingOffset >= (GlobalHUDSubsystem->UiOffset / GlobalHUDSubsystem->UISpeed) * MusicGlobalSpeed)
			{
				IsLerpingOffset = false;
			}
		}
		else
		{
			Tempo += DeltaTime * MusicGlobalSpeed;
			
			if (!CurrentSkeleton) return; // Secu check if current skeleton is set

			// F_Note* CurrentNote = GetCurrentWaitingNote();
			// UMusicNote* CurrentNoteSlot = GlobalHUDSubsystem->NotesInstanciated[CurrentWaitingNoteIndex];

			// Not yet time for qte => !IsAwaitingReply
			if (Tempo < ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * MusicGlobalSpeed))
			{
				IsAwaitingReply = false;
				return;
			}
			
			// Is Awaiting Reply
			if (Tempo >= ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * MusicGlobalSpeed) && !IsAwaitingReply)
			{
				IsAwaitingReply = true;
				return;
			}
			
			// check success
			if (Tempo >= ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency + TimeTolerance) * MusicGlobalSpeed))
			{
				IsAwaitingReply = false;

				// success
				if(HasAchievedQte())
				{
					//Melodie finie et réussie
					if (CurrentWaitingNoteIndex == CurrentSkeleton->MySkeleton->Notes.Num()-1)
					{
						SucceedMelody();
					}
					// go next note
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Go Next Note")), true, FVector2D(2, 2));

						// Increment the FailNotePossible to give back chance to player.
						// If it's already at the max then limit the CurrentFailNotePossible at the Max
						CurrentFailNotePossible++;

						if (CurrentFailNotePossible >= MaxFailNotePossible)
						{
							CurrentFailNotePossible = MaxFailNotePossible;
						}
						
						Tempo = TimeTolerance * MusicGlobalSpeed;
						SetCurrentWaitingNoteIndex(GetCurrentWaitingNoteIndex() + 1);
					}
				}
				// lost qte time
				else
				{
					LostQTE();
				}
				
				// reset
				ResetMusicianReply();
			}
		}
	}
}

#pragma region Skeletons&Notes
F_Note* UMusicWorldSubsystem::GetCurrentWaitingNote() const
{
	if (CurrentWaitingNoteIndex > CurrentSkeleton->MySkeleton->Notes.Num()-1)
		UE_LOG(LogTemp, Error, TEXT("Current waiting Note is out of range"));
	
	return &CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex];
}

int UMusicWorldSubsystem::GetCurrentWaitingNoteIndex() const
{
	return CurrentWaitingNoteIndex;
}

void UMusicWorldSubsystem::SetCurrentWaitingNoteIndex(int NewIndex)
{
	CurrentWaitingNoteIndex = NewIndex;	
}

void UMusicWorldSubsystem::ReceivedMusicianInput()
{
	if (HasMusicianReceivedInput) return;
	HasMusicianReceivedInput = true;
}

void UMusicWorldSubsystem::ResetMusicianReply()
{
	HasMusicianReceivedInput = false;
}

#pragma endregion


#pragma region Music Mechanic
void UMusicWorldSubsystem::InitMusic(ASkeletonController* Skeleton)
{
	MelodyState = EMelodyType::NONE;
	
	// Init Data
	CurrentSkeleton = Skeleton;

	// Init InValues
	CurrentCursorValue = 0.f;

	// Spawn Notes in UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->SpawnNotesPartition(CurrentSkeleton);
	
	IsInWorldStateMusic = true;
	
	// Initialize CurrentFailNotePossible.
	CurrentFailNotePossible = MaxFailNotePossible;
	
	StartCountDown();
}

void UMusicWorldSubsystem::SucceedMelody()
{
	// DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));

	// SUCCEED
	MelodyState = EMelodyType::SUCCEED;
	
	// Reset Music
	IsInWorldStateMusic = false;
	Tempo = 0.f;
	CurrentWaitingNoteIndex = 0;
	
	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();
	
	// Camera
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetGlobalCamera();
}

void UMusicWorldSubsystem::LostMelody()
{
	// DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie raté")), true, FVector2D(2, 2));

	// FAILED
	MelodyState = EMelodyType::FAILED;
	
	// Reset Music
	IsInWorldStateMusic = false;
	Tempo = 0.f;
	CurrentWaitingNoteIndex = 0;
	
	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();
	
	// Camera
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetGlobalCamera();

}

bool UMusicWorldSubsystem::HasAchievedQte()
{
	// UMusicNote* CurrentNoteSlot = GlobalHUDSubsystem->NotesInstanciated[CurrentWaitingNoteIndex];
	// USlider* Slider = GlobalHUDSubsystem->WBPResurrectionInstance->PitchSlider;
	//
	// if (!Slider || !CurrentNoteSlot || !GetCurrentWaitingNote())
	// {
	// 	UE_LOGFMT(LogTemp, Error, "Has not achived QTE because one reference or several references are null ! ");
	// 	return false;
	// }
	//
	//  IsConductorOnTheRightPitch = GetCurrentWaitingNote()->Pitch >= CurrentCursorValue - PitchTolerance
	// 	&& GetCurrentWaitingNote()->Pitch <= CurrentCursorValue + PitchTolerance;
	//
	// if (HasMusicianReceivedInput)
	// {
	// 	GlobalHUDSubsystem->SetObjectColor<UMusicNote>(CurrentNoteSlot, FColor::Green);
	// }
	// else
	// {
	// 	GlobalHUDSubsystem->SetObjectColor<UMusicNote>(CurrentNoteSlot, FColor::Red);
	// }
	//
	// if (IsConductorOnTheRightPitch)
	// {
	// 	GlobalHUDSubsystem->SetObjectColor<USlider>(Slider, FColor::Green);
	// }
	// else
	// {
	// 	GlobalHUDSubsystem->SetObjectColor<USlider>(Slider, FColor::Red);
	// }
	//
	// if (HasMusicianReceivedInput && IsConductorOnTheRightPitch)
	// {
	// 	return true;
	// }
	//
	// return false;

	return true;
}

void UMusicWorldSubsystem::LostQTE()
{
	// EDITING MAIN MECHANIC
	// // Going from 2 previous notes without exceed 0.
	// int RewindNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex - 2);
	// // Rewind the partition in UI.
	// GlobalHUDSubsystem->RewindPartition(RewindNoteIndex, CurrentSkeleton->MySkeleton->Notes[RewindNoteIndex]);
	// // Change back to blue the color of the Slot Note.
	// UMusicNote* RewindNoteSlote1 = GlobalHUDSubsystem->NotesInstanciated[RewindNoteIndex];
	// if (!RewindNoteSlote1) return;
	// UMusicNote* RewindNoteSlote2 = GlobalHUDSubsystem->NotesInstanciated[RewindNoteIndex + 1];
	// if (!RewindNoteSlote2) return;
	// RewindNoteSlote1->NoteImage->SetColorAndOpacity(FLinearColor::Blue);
	// RewindNoteSlote2->NoteImage->SetColorAndOpacity(FLinearColor::Blue);
	// // Set the CurrentNoteIndex to the NewNote after going to 2 previous notes.
	// SetCurrentWaitingNoteIndex(RewindNoteIndex);
	// // Restart countdown.
	// StartCountDown();
	// // Set Tempo to the Note you have to play - PreviewTime.
	// Tempo = (CurrentSkeleton->MySkeleton->Notes[GetCurrentWaitingNoteIndex()].Frequency - GlobalHUDSubsystem->PreviewTime) * MusicGlobalSpeed;

	
	// Go down of one note possible when you failed the qte.
	CurrentFailNotePossible--;

	// Because we have to go to the next note if we lost the qte we have to give to the tempo the TimeTolerance
	// It's only if we reach the MaxPossibleFailNote that we lost.
	Tempo = TimeTolerance * MusicGlobalSpeed;

	// Increment to the next note.
	if (GetCurrentWaitingNoteIndex() < CurrentSkeleton->MySkeleton->Notes.Num() - 1)
	{
		SetCurrentWaitingNoteIndex(GetCurrentWaitingNoteIndex() + 1);
	}
	
	// If the max note possible to fail has been achieved you go out of the music state without the skeletons.
	if (CurrentFailNotePossible <= 0)
	{
		LostMelody();
	}
}

EMelodyType UMusicWorldSubsystem::GetMelodyType() const
{
	return MelodyState;
}

void UMusicWorldSubsystem::StartCountDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Start CountDown de 3 sec")), true, FVector2D(2, 2));
	
	Tempo = 0.f;
	IsInCountDown = true;
}

#pragma endregion
