// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicWorldSubsystem.h"

#include "InputAction.h"
#include "Components/Slider.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Data/MusicGenericData.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "Sound/SoundCue.h"
#include "UI/GlobalHUDSubsystem.h"
#include "UI/UResurrectionWidget.h"


// ---- SUBSYSTEM OVERRIDE ---- 
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
	GlobalHUDSubsystem->SetMusicWorldSubsystem(this);

	// Init Subsystem Settings.
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;

	// Get the Music Generic Data.
	TObjectPtr<UMusicGenericData> MusicGenericData = SubsystemSettings->MusicGenericData.LoadSynchronous();
	if (!MusicGenericData) return;

	// Initialize TimeTolerance.
	TimeTolerance = MusicGenericData->TimeTolerance;

	// Initialize MaxFailNotePossible.
	MaxFailNotePossible = MusicGenericData->MaxFailNotePossible;

	// Initialize PitchTolerance.
	PitchTolerance = MusicGenericData->PitchTolerance;

	// Initialize FailedNoteSound.
	FailedNoteSound = MusicGenericData->FailedNoteSound;
}

void UMusicWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	// Security Check: The Music is supposed to work only in WorldStateMusic.
	if (!IsInWorldStateMusic) return;

	if (IsInCountDown)
	{
		DecreaseTimerCountdown(DeltaTime);
		
		if (HasFinishedCountdown())
		{
			FinishCountDown();
		}
	}
	else
	{
		// Security Check: The Music Logic can't work without GlobalHUDSubsystem.
		if (!GlobalHUDSubsystem)
		{
			UE_LOGFMT(LogTemp, Error, "GlobalHUDSubsystem is null !");
			return;
		}

		GlobalHUDSubsystem->MovePartition(DeltaTime);

		if (IsLerpingOffset)
		{
			IncreaseTimerLerpingOffset(DeltaTime);
			
			if (HasFinishedLerpingOffset())
			{
				IsLerpingOffset = false;
			}
		}
		else
		{
			IncreaseMusicTempo(DeltaTime);
			IncreaseMusicTempoUI(DeltaTime);

			// Security Check: The Music Logic can't work if there is no skeleton. 
			if (!CurrentSkeleton)
			{
				UE_LOGFMT(LogTemp, Error, "Current Skeleton is null !");
				return;
			}

			// The time to do the QTE hasn't come yet.
			if (IsBeforeWindowNote())
			{
				IsAwaitingReply = false;
			}

			// Is it the time to do the QTE.
			if (HasEnteredWindowNote())
			{
				IsAwaitingReply = true;
			}

			// Reach the frequency !
			if (HasReachPitchSlider() && !HasReachFrequency)
			{
				HasReachFrequency = true;

				if (GetCurrentWaitingNoteIndexUI() < CurrentSkeleton->MySkeleton->Notes.Num() - 1)
				{
					PreviousCurrentWaitingNoteIndexUI = CurrentWaitingNoteIndexUI;
					CurrentWaitingNoteIndexUI++;
					
					if (CurrentWaitingNoteIndexUI == PreviousCurrentWaitingNoteIndexUI)
					{
						CurrentWaitingNoteIndexUI++;
					}
					
					TempoNoteUI = 0;
				}
			}
			else
			{
				UE_LOGFMT(LogTemp, Warning, "HasReachPitch : false");
			}
			
			// Check for the exit of the window note, to check if the player HasAchievedQTE.
			if (HasExitedWindowNote())
			{
				// Not Time for the QTE anymore.
				IsAwaitingReply = false;

				// success
				if(HasAchievedQte())
				{
					if (HasFinishedMelody())
					{
						SucceedMelody();
					}
					// It's not the end of the partition, the player succeed the QTE and we go to the next Note.
					else
					{
						SucceedQTE();
					}
				}
				else
				{
					LostQTE();
				}
				
				ResetMusicianReply();
			}
		}
	}
}


// ---- NOTES & SKELETONS ---- 
F_Note* UMusicWorldSubsystem::GetCurrentWaitingNote() const
{
	if (GetCurrentWaitingNoteIndex() > CurrentSkeleton->MySkeleton->Notes.Num() - 1) UE_LOGFMT(LogTemp, Error, "ERROR: Current waiting Note is out of range !");
	return &CurrentSkeleton->MySkeleton->Notes[GetCurrentWaitingNoteIndex()];
}

UMusicNote* UMusicWorldSubsystem::GetCurrentWaitingNoteWidget() const
{
	if (GetCurrentWaitingNote() == nullptr)
	{
		UE_LOGFMT(LogTemp, Error, "ERROR: No current waiting note !");
		return nullptr;
	}
	if (GlobalHUDSubsystem->WBPResurrectionInstance == nullptr)
	{
		UE_LOGFMT(LogTemp, Error, "ERROR: WBPResurrectionInstance == nullptr !");
		return nullptr;
	}
	
	return GlobalHUDSubsystem->NotesInstanciated[GetCurrentWaitingNoteIndex()];
}

int UMusicWorldSubsystem::GetCurrentWaitingNoteIndex() const
{
	return CurrentWaitingNoteIndex;
}

void UMusicWorldSubsystem::SetCurrentWaitingNoteIndex(int NewIndex)
{
	CurrentWaitingNoteIndex = NewIndex;	
}

int UMusicWorldSubsystem::GetCurrentWaitingNoteIndexUI() const
{
	return CurrentWaitingNoteIndexUI;
}

F_Note* UMusicWorldSubsystem::GetCurrentWaitingNoteUI() const
{
	if (GetCurrentWaitingNoteIndexUI() > CurrentSkeleton->MySkeleton->Notes.Num() - 1) UE_LOGFMT(LogTemp, Error, "ERROR: Current waiting Note is out of range !");
	return &CurrentSkeleton->MySkeleton->Notes[GetCurrentWaitingNoteIndexUI()];
}

bool UMusicWorldSubsystem::GetIsConductorOnPitch() const
{
	return IsConductorOnTheRightPitch;
}

void UMusicWorldSubsystem::ReceivedMusicianInput()
{
	if (HasMusicianReceivedInput) return;
	HasMusicianReceivedInput = true;
}

void UMusicWorldSubsystem::ResetMusicianReply()
{
	HasMusicianReceivedInput = false;
	HasReachFrequency = false;
}


// ---- MUSIC LOGIC ----
void UMusicWorldSubsystem::InitMusic(ASkeletonController* Skeleton)
{
	MelodyState = EMelodyType::NONE;
	
	// Init the Skeleton for the Music Logic.
	CurrentSkeleton = Skeleton;

	Tempo = 0.f;
	TempoNoteUI = 0.f;
	TimerCountDown = 3.f;

	CurrentWaitingNoteIndex = 0;
	CurrentWaitingNoteIndexUI = 0;
	
	IsAwaitingReply = false;
	HasMusicianReceivedInput = false;
	HasReachFrequency = false;
	
	// Reset the current cursor value for the pith slider.
	CurrentPitchCursorValue = 0.f;
	
	IsLerpingOffset = true;
	TimerLerpingOffset = 0.f;
	
	IsConductorOnTheRightPitch = false;
	HasLostMelody = false;
	IsInWorldStateMusic = true;
	
	// Spawn Notes in UI.
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->SpawnNotesPartition(CurrentSkeleton);
	
	// Initialize CurrentFailNotePossible.
	SetCurrentFailNotePossible(MaxFailNotePossible);
	
	StartCountDown();
}

bool UMusicWorldSubsystem::IsBeforeWindowNote() const
{
	return Tempo < (GetCurrentWaitingNote()->Frequency - TimeTolerance);
}

bool UMusicWorldSubsystem::HasEnteredWindowNote() const
{
	return Tempo >= (GetCurrentWaitingNote()->Frequency - TimeTolerance);
}

bool UMusicWorldSubsystem::HasExitedWindowNote() const
{
	return Tempo >= (GetCurrentWaitingNote()->Frequency + TimeTolerance);
}

bool UMusicWorldSubsystem::HasFinishedMelody() const
{
	return GetCurrentWaitingNoteIndex() == CurrentSkeleton->MySkeleton->Notes.Num() - 1;
}

void UMusicWorldSubsystem::SucceedQTE()
{
	// POSITIVE feedback
	if (GetCurrentWaitingNote()->Sound)
		UGameplayStatics::PlaySound2D(GetWorld(),GetCurrentWaitingNote()->Sound);

	// Continue
	GoNextNote();
}

void UMusicWorldSubsystem::SucceedMelody()
{
	// DEBUG
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie finie et réussie")), true, FVector2D(2, 2));

	// SUCCEED
	MelodyState = EMelodyType::SUCCEED;
	
	// Reset Music
	IsInWorldStateMusic = false;
	// BackgroundAudioComponent->SetActive(false);
	
	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();

	// Animation of Enter
	GlobalHUDSubsystem->DisplayPartitionFinish("SUCCEED MELODY");

	/// TO EDIT don't use delay
	FTimerHandle IsAnimationFinished;
	GetWorld()->GetTimerManager().ClearTimer(IsAnimationFinished);

	GetWorld()->GetTimerManager().SetTimer(
		IsAnimationFinished, [this]()
		{
			GlobalHUDSubsystem->RemovePartitionFinish();

			// Camera
			GlobalGameSubsystem->SetWorldTransportState();
		},
		2.f,
		false
	);
}

void UMusicWorldSubsystem::LostMelody()
{
	// DEBUG
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, FString::Printf(TEXT("Melodie raté")), true, FVector2D(2, 2));

	if (HasLostMelody) return;
	HasLostMelody = true;
	
	// FAILED
	MelodyState = EMelodyType::FAILED;
	
	// Reset Music
	IsInWorldStateMusic = false;
	// BackgroundAudioComponent->SetActive(false);
	
	// UI
	UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>()->RemoveResurrectionWidget();

	// Animation of Exit
	GlobalHUDSubsystem->DisplayPartitionFinish("FAILED MELODY");

	/// TO EDIT don't use delay
	FTimerHandle IsAnimationFinished;
	GetWorld()->GetTimerManager().ClearTimer(IsAnimationFinished);

	GetWorld()->GetTimerManager().SetTimer(
		IsAnimationFinished, [this]()
		{
			GlobalGameSubsystem->SetLostMelody();
		},
		2.f,
		false
		);
}

void UMusicWorldSubsystem::SetNoteFeedbackMusic(FLinearColor NewColor) const
{
	UResurrectionWidget* ResurrectionWidget = GlobalHUDSubsystem->WBPResurrectionInstance;
	if (!ResurrectionWidget) return;

	UImage* CurrentNoteFeedback = ResurrectionWidget->GetFeedbackPosFromInputPitch(GetCurrentWaitingNoteUI()->Pitch);
	GlobalHUDSubsystem->SetImageColor(CurrentNoteFeedback, NewColor);
}

bool UMusicWorldSubsystem::HasAchievedQte()
{
	USlider* Slider = GlobalHUDSubsystem->WBPResurrectionInstance->PitchSlider;
	
	if (!Slider || !GetCurrentWaitingNoteWidget() || !GetCurrentWaitingNote())
	{
		UE_LOGFMT(LogTemp, Error, "ERROR: Has not achieved QTE because one reference or several references are null !");
		return false;
	}
	
	IsConductorOnTheRightPitch = GetCurrentWaitingNote()->Pitch >= GetCurrentPitchCursorValue() - PitchTolerance
	   && GetCurrentWaitingNote()->Pitch <= GetCurrentPitchCursorValue() + PitchTolerance;
	
	if (HasMusicianReceivedInput && IsConductorOnTheRightPitch)
	{
		return true;
	}
	
	return false;
}

void UMusicWorldSubsystem::LostQTE()
{
	// FAILS 
	SetCurrentFailNotePossible(GetCurrentFailNotePossible() - 1);

	if (GetCurrentWaitingNoteWidget() != nullptr)
	{
		GetCurrentWaitingNoteWidget()->NoteImage->SetColorAndOpacity(FLinearColor::Red);

		FTimerHandle NoteChangeBackColor;
		GetWorld()->GetTimerManager().ClearTimer(NoteChangeBackColor);

		GetWorld()->GetTimerManager().SetTimer(
			NoteChangeBackColor, [this]()
			{
				if (GetCurrentWaitingNoteWidget() != nullptr)
				{
					GetCurrentWaitingNoteWidget()->NoteImage->SetColorAndOpacity(FLinearColor::White);
				}
			},
			2.f,
			false
			);
	}
	
	// If the max note possible to fail has been achieved you go out of the music state without the skeletons.
	if (HasLostAllFaileNotePossible())
	{
		SetCurrentFailNotePossible(0);
		LostMelody();
		return;
	}

	if (HasFinishedMelody())
	{
		SucceedMelody();
		return;
	}

	// continue 
	GoNextNote();
}

int UMusicWorldSubsystem::GetCurrentFailNotePossible() const
{
	return CurrentFailNotePossible;
}

void UMusicWorldSubsystem::SetCurrentFailNotePossible(float NewValue)
{
	CurrentFailNotePossible = NewValue;
}

bool UMusicWorldSubsystem::HasLostAllFaileNotePossible() const
{
	return GetCurrentFailNotePossible() <= 0;
}

void UMusicWorldSubsystem::IncreaseMusicTempo(float DeltaTime)
{
	Tempo += DeltaTime;
}

void UMusicWorldSubsystem::IncreaseMusicTempoUI(float DeltaTime)
{
	TempoNoteUI += DeltaTime;
}

void UMusicWorldSubsystem::GoNextNote()
{
	if (GetCurrentWaitingNoteIndex() >= CurrentSkeleton->MySkeleton->Notes.Num() - 1) return;
	Tempo = TimeTolerance;
	SetCurrentWaitingNoteIndex(GetCurrentWaitingNoteIndex() + 1);
}

float UMusicWorldSubsystem::GetCurrentPitchCursorValue() const
{
	return CurrentPitchCursorValue;
}

void UMusicWorldSubsystem::SetCurrentPitchCursorValue(float NewPitchCursorValue)
{
	CurrentPitchCursorValue = NewPitchCursorValue;
}

float UMusicWorldSubsystem::GetPitchTolerance() const
{
	return PitchTolerance;
}

bool UMusicWorldSubsystem::GetIsAwatingReply() const
{
	return IsAwaitingReply;
}

EMelodyType UMusicWorldSubsystem::GetMelodyType() const
{
	return MelodyState;
}


// ---- MUSIC COUNTDOWN ----
void UMusicWorldSubsystem::StartCountDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Start CountDown de 3 sec")), true, FVector2D(2, 2));
	
	Tempo = 0.f;
	IsLerpingOffset = true;
	TimerLerpingOffset = 0.f;
	
	IsInCountDown = true;
}

void UMusicWorldSubsystem::FinishCountDown()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Finish CountDown")), true, FVector2D(2, 2));
	IsInCountDown = false;
	TimerCountDown = 3.f;

	// Background Music
	// BackgroundAudioComponent = UGameplayStatics::SpawnSound2D(GetWorld(), CurrentSkeleton->MySkeleton->BackgroundMusic);
	// BackgroundAudioComponent->SetActive(true);
}

float UMusicWorldSubsystem::GetTimerCountdown() const
{
	return TimerCountDown;
}

void UMusicWorldSubsystem::DecreaseTimerCountdown(float DeltaTime)
{
	TimerCountDown -= DeltaTime;
}

bool UMusicWorldSubsystem::HasFinishedCountdown() const
{
	return GetTimerCountdown() <= 0.f;
}


// ---- MUSIC UI TIMER ----
float UMusicWorldSubsystem::GetTimerLerpingOffset() const
{
	return TimerLerpingOffset;
}

void UMusicWorldSubsystem::IncreaseTimerLerpingOffset(float DeltaTime)
{
	TimerLerpingOffset += DeltaTime;
}

bool UMusicWorldSubsystem::HasFinishedLerpingOffset() const
{
	return GetTimerLerpingOffset() >= (GlobalHUDSubsystem->GetUIOffset() / GlobalHUDSubsystem->GetUISpeed());
}

bool UMusicWorldSubsystem::HasReachPitchSlider() const
{
	bool HasReachPitchSlider;
	
	if (GetCurrentWaitingNoteIndex() <= 0)
	{
		HasReachPitchSlider = TempoNoteUI >= (GlobalHUDSubsystem->GetUIOffset() / GlobalHUDSubsystem->GetUISpeed());
	}
	else
	{
		HasReachPitchSlider = TempoNoteUI >= GetCurrentWaitingNote()->Frequency;
	}

	return HasReachPitchSlider;
}



