// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateMusic.h"
#include "InputActionValue.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterInputData.h"
#include "Data/F_Note.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Music/MusicWorldSubsystem.h"
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

void UPipouCharacterStateMusic::SetMusicManager()
{
	MusicWorldSubsystem = GetWorld()->GetSubsystem<UMusicWorldSubsystem>();
}

// Event Delegates
void UPipouCharacterStateMusic::OnCharacterPitch(FInputActionValue InputActionValue)
{
	if (CurrentRole == EPipouCharacterRoles::Conductor)
	{
		UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
		if (!HUDSubsystem || !HUDSubsystem->WBPResurrectionInstance) return;
		
		// Dead Zone
		if (InputActionValue.Get<float>() >= -0.1f && InputActionValue.Get<float>() <= 0.1f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(0.f);

			// Update slider too
			HUDSubsystem->WBPResurrectionInstance->SetSliderPitch(0.f);
			return;
		}

		// Le pitch est supérieur à 0.2 et inférieur à 0.8, il est entre 0.3 et 0.7
		if (InputActionValue.Get<float>() > 0.2f && InputActionValue.Get<float>() < 0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(0.5f);
		}
		// Le pitch est supérieur ou égale à 0.8. Il est entre 0.8 et 1
		else if (InputActionValue.Get<float>() >= 0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(1.f);
		}
		// Le pitch est inférieur à -0.2 et supérieur à -0.8. Il est entre -0.3 et -0.7
		else if (InputActionValue.Get<float>() < -0.2f && InputActionValue.Get<float>() > -0.8f)
		{
			MusicWorldSubsystem->SetCurrentPitchCursorValue(-0.5f);
		}
		// Le pitch est inférieur ou égale à -0.8. Il est entre -0.8 et -1
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
	MusicWorldSubsystem->SetCurrentPitchCursorValue(0);
}

void UPipouCharacterStateMusic::OnCharacterPressedNote(UInputAction* InputAction)
{
	if (CurrentRole == EPipouCharacterRoles::Musician)
	{
		UGlobalHUDSubsystem* HUDSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
		if (!HUDSubsystem || !HUDSubsystem->WBPResurrectionInstance) return;
		
		if (MusicWorldSubsystem->GetIsAwatingReply() && MusicWorldSubsystem->GetCurrentWaitingNote()->InputAction == InputAction && !HasPressedNotes
				&& (MusicWorldSubsystem->GetCurrentWaitingNote()->Pitch >= MusicWorldSubsystem->GetCurrentPitchCursorValue() - MusicWorldSubsystem->GetPitchTolerance()
				&& MusicWorldSubsystem->GetCurrentWaitingNote()->Pitch <= MusicWorldSubsystem->GetCurrentPitchCursorValue() + MusicWorldSubsystem->GetPitchTolerance()))
		{
			HasPressedNotes = true;
			MusicWorldSubsystem->ReceivedMusicianInput();

			// Set invisibility for the notes.
			MusicWorldSubsystem->GetCurrentWaitingNoteWidget()->PlayValidationNote({50, 50}, 0);
			MusicWorldSubsystem->SetBehindNoteFeedback(FLinearColor::Green);
		}
		else if (!MusicWorldSubsystem->GetIsAwatingReply() && !MusicWorldSubsystem->IsInCountDown && !HasPressedNotes)
		{
			HasPressedNotes = true;

			// Negative feedback
			UGameplayStatics::PlaySound2D(GetWorld(), MusicWorldSubsystem->FailedNoteSound);
			MusicWorldSubsystem->GetCurrentWaitingNoteWidget()->PlayFailNote();
			MusicWorldSubsystem->SetBehindNoteFeedback(FLinearColor::Red);
			
			// FAILS
			MusicWorldSubsystem->SetCurrentFailNotePossible(MusicWorldSubsystem->GetCurrentFailNotePossible() - 1);
			HUDSubsystem->ApplyMistakeIncrease(MusicWorldSubsystem->GetCurrentFailNotePossible(), MusicWorldSubsystem->MaxFailNotePossible);

			// Defeat
			if (MusicWorldSubsystem->HasLostAllFaileNotePossible())
			{
				MusicWorldSubsystem->SetCurrentFailNotePossible(0);
				MusicWorldSubsystem->LostMelody();
			}
		}
	}
}




