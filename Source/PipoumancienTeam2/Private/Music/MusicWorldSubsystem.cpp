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

	// Initialize
	PitchSpeed = SubsystemSettings->PitchSpeed;
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

			F_Note* CurrentNote = GetCurrentWaitingNote();
			UMusicNote* CurrentNoteSlot = GlobalHUDSubsystem->NotesInstanciated[CurrentWaitingNoteIndex];

			// Not yet time for qte => !IsAwaitingReply
			if (Tempo < ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * MusicGlobalSpeed))
			{
				IsAwaitingReply = false;
				return;
			}
			
			// Is Awaiting Reply
			if (Tempo >= ((CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Frequency - TimeTolerance) * MusicGlobalSpeed) && !IsAwaitingReply)
			{
				// GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Red, FString::Printf(TEXT("INPUT : %s"), *CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].InputAction->GetName()), true, FVector2D(2, 2));
				// GEngine->AddOnScreenDebugMessage(-1, TimeTolerance * 2, FColor::Blue, FString::Printf(TEXT("PITCH : %f"), CurrentSkeleton->MySkeleton->Notes[CurrentWaitingNoteIndex].Pitch), true, FVector2D(2, 2));

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
						FinishMelody();
					}
					// go next note
					else
					{
						GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Go Next Note")), true, FVector2D(2, 2));
						
						Tempo = TimeTolerance * MusicGlobalSpeed;
						CurrentWaitingNoteIndex++;
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
	// Init Data
	CurrentSkeleton = Skeleton;

	// Init InValues
	CurrentCursorValue = 0.f;

	// Spawn Notes in UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->SpawnNotesPartition(CurrentSkeleton);
	
	IsInWorldStateMusic = true;
	
	StartCountDown();
}

void UMusicWorldSubsystem::FinishMelody()
{
	// DEBUG
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));

	// Reset Music
	IsInWorldStateMusic = false;
	Tempo = 0.f;
	CurrentWaitingNoteIndex = 0;
	
	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();
	
	// Camera
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->CallCamera(ECameraType::GlobalCamera);
}

bool UMusicWorldSubsystem::HasAchievedQte()
{
	UMusicNote* CurrentNoteSlot = GlobalHUDSubsystem->NotesInstanciated[CurrentWaitingNoteIndex];
	USlider* Slider = GlobalHUDSubsystem->WBPResurrectionInstance->PitchSlider;

	if (!Slider || !CurrentNoteSlot || !GetCurrentWaitingNote())
	{
		UE_LOGFMT(LogTemp, Error, "Has not achived QTE because one reference or several references are null ! ");
		return false;
	}
	
	 IsConductorOnTheRightPitch = GetCurrentWaitingNote()->Pitch >= CurrentCursorValue - PitchTolerance
		&& GetCurrentWaitingNote()->Pitch <= CurrentCursorValue + PitchTolerance;

	if (HasMusicianReceivedInput)
	{
		GlobalHUDSubsystem->SetObjectColor<UMusicNote>(CurrentNoteSlot, FColor::Green);
	}
	else
	{
		GlobalHUDSubsystem->SetObjectColor<UMusicNote>(CurrentNoteSlot, FColor::Red);
	}

	if (IsConductorOnTheRightPitch)
	{
		GlobalHUDSubsystem->SetObjectColor<USlider>(Slider, FColor::Green);
	}
	else
	{
		GlobalHUDSubsystem->SetObjectColor<USlider>(Slider, FColor::Red);
	}
	
	if (HasMusicianReceivedInput && IsConductorOnTheRightPitch)
	{
		return true;
	}
	
	return false;
}

void UMusicWorldSubsystem::LostQTE()
{
	// Going from 2 previous notes without exceed 0.
	int RewindNoteIndex = FMath::Max(0, CurrentWaitingNoteIndex - 2);

	// Rewind the partition in UI.
	GlobalHUDSubsystem->RewindPartition(RewindNoteIndex, CurrentSkeleton->MySkeleton->Notes[RewindNoteIndex]);

	// Set the CurrentNoteIndex to the NewNote after going to 2 previous notes.
	SetCurrentWaitingNoteIndex(RewindNoteIndex);

	// Restart countdown.
	StartCountDown();
}

void UMusicWorldSubsystem::StartCountDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Start CountDown de 3 sec")), true, FVector2D(2, 2));
	
	Tempo = 0.f;
	IsInCountDown = true;
}

#pragma endregion
