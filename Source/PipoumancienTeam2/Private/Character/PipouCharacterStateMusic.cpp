// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"
#include "InputActionValue.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterInputData.h"
#include "Data/F_Note.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Music/MusicWorldSubsystem.h"
#include "Settings/SubsystemSettings.h"
#include "Sound/SoundCue.h"
#include "UI/GlobalHUDSubsystem.h"
#include "UI/UResurrectionWidget.h"

EPipouCharacterStateID UPipouCharacterStateMusic::GetStateID()
{
	return EPipouCharacterStateID::Music;
}

void UPipouCharacterStateMusic::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);

	InitRole();
	InitSkeletons();
	InitInputPitch();
	InitSliderPitchSpeed();
	SetMusicManager();

	Character->InputPressedNoteEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputTriggeredNoteEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputPitchEvent.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitch);
	Character->InputPitchCompleted.AddDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitchCompleted);

	// ANIMS
	Character->GetMesh()->PlayAnimation(MusicAnim,true);
}

void UPipouCharacterStateMusic::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
}

void UPipouCharacterStateMusic::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);

	Character->InputPressedNoteEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputTriggeredNoteEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPressedNote);
	Character->InputPitchEvent.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitch);
	Character->InputPitchCompleted.RemoveDynamic(this, &UPipouCharacterStateMusic::OnCharacterPitchCompleted);
}

// Music
void UPipouCharacterStateMusic::InitRole()
{
	switch (Character->GetPipouCharacterClass())
	{
	case EPipouCharacterClass::Necro:
		CurrentRole = EPipouCharacterRoles::Musician;
		break;
		
	case EPipouCharacterClass::Phantom:
		CurrentRole = EPipouCharacterRoles::Conductor;
		break;
		
	default:
		CurrentRole = EPipouCharacterRoles::None;
	}
}

void UPipouCharacterStateMusic::InitSkeletons()
{
	Skeleton = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>()->GetCurrentSkeleton();
}

void UPipouCharacterStateMusic::InitInputPitch()
{
	InputPitch = Character->InputData->InputPitch;
}

void UPipouCharacterStateMusic::InitSliderPitchSpeed()
{
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	if (!SubsystemSettings) return;
	//
	// MaxPitchSpeed = SubsystemSettings->MaxSpeedPitch;
	// AccelerationPitchSpeed = SubsystemSettings->AccelerationPitchSpeed;
}

void UPipouCharacterStateMusic::SetMusicManager()
{
	MusicWorldSubsystem = GetWorld()->GetSubsystem<UMusicWorldSubsystem>();
}

// Event Delegates
void UPipouCharacterStateMusic::OnCharacterPitch(FInputActionValue InputActionValue)
{
	if (CurrentRole == EPipouCharacterRoles::Conductor)
	{
		// Dead Zone
		if (InputActionValue.Get<float>() >= -0.1f && InputActionValue.Get<float>() <= 0.1f) return;

		UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
		if (!HUDSubsystem || !HUDSubsystem->WBPResurrectionInstance) return;

		if (InputActionValue.Get<float>() >= 0.1f && InputActionValue.Get<float>() <= 0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(0.5f);
		}
		else if (InputActionValue.Get<float>() >= 0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(1.f);
		}
		else if (InputActionValue.Get<float>() <= -0.1f && InputActionValue.Get<float>() >= -0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(-0.5f);
		}
		else if (InputActionValue.Get<float>() <= -0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(-1.f);
		}

		HUDSubsystem->WBPResurrectionInstance->SetSliderPitch(MusicWorldSubsystem->GetCurrentPitchCursorValue());
	}
}

void UPipouCharacterStateMusic::OnCharacterPitchCompleted()
{
	UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
	if (!HUDSubsystem || !HUDSubsystem->WBPResurrectionInstance) return;
	
	HUDSubsystem->WBPResurrectionInstance->SetSliderPitch(0);
}

void UPipouCharacterStateMusic::OnCharacterPressedNote(UInputAction* InputAction)
{
	if (CurrentRole == EPipouCharacterRoles::Musician)
	{
		if (MusicWorldSubsystem->GetIsAwatingReply() && MusicWorldSubsystem->GetCurrentWaitingNote()->InputAction == InputAction)
		{
			HasPressedNotes = true;
			MusicWorldSubsystem->ReceivedMusicianInput();
			MusicWorldSubsystem->SetNoteFeedbackMusic(FLinearColor::Green);
			
			// Set invisibility for the notes.
			MusicWorldSubsystem->GetCurrentWaitingNoteWidget()->NoteImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f,0.f));
			MusicWorldSubsystem->GetCurrentWaitingNoteWidget()->LetterText->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f,0.f));
		}
		else if (!MusicWorldSubsystem->GetIsAwatingReply() && !MusicWorldSubsystem->IsInCountDown && !HasPressedNotes)
		{
			HasPressedNotes = true;
			
			// Negative feedback
			UGameplayStatics::PlaySound2D(GetWorld(), MusicWorldSubsystem->FailedNoteSound);
			MusicWorldSubsystem->SetNoteFeedbackMusic(FLinearColor::Red);

			// FAILS
			MusicWorldSubsystem->SetCurrentFailNotePossible(MusicWorldSubsystem->GetCurrentFailNotePossible() - 1);

			// Defeat
			if (MusicWorldSubsystem->HasLostAllFaileNotePossible())
			{
				MusicWorldSubsystem->SetCurrentFailNotePossible(0);
				MusicWorldSubsystem->LostMelody();
			}
		}
	}
}




