// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GlobalGameSubsystem.h"

#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Character/PipouCharacterStateID.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Music/MusicWorldSubsystem.h"
#include "UI/GlobalHUDSubsystem.h"

// to call in init pipou chara
void UGlobalGameSubsystem::SetCharacters(APipouCharacter* Character)
{
	PipouCharacters.Add(Character);
}

F_Skeleton* UGlobalGameSubsystem::GetCurrentSkeleton() const
{
	return CurrentSkeleton;
}

void UGlobalGameSubsystem::SetCurrentSkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
}

// Music
void UGlobalGameSubsystem::AddNote(UInputAction* InputAction)
{
	InputPressed.Add(InputAction);

	if (InputPressed.Num() >= NbNotesToCheck)
	{
		if (HasValidFirstNotes()) 
			SetWorldMusicState();
		
		ResetInputsArray();
	}
}

bool UGlobalGameSubsystem::HasValidFirstNotes()
{
	for (int i = 0; i < NbNotesToCheck; ++i)
	{
		if (CurrentSkeleton->Notes[i].InputAction != InputPressed[i])
		{
			// UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes raté"));
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Enchainement de 3 notes raté")), true, FVector2D(2, 2));
			
			return false;
		}
	}

	// UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes réussi"));
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Enchainement de 3 notes réussi")), true, FVector2D(2, 2));
	
	return true;
}

void UGlobalGameSubsystem::ResetInputsArray()
{
	InputPressed.Empty();
}

void UGlobalGameSubsystem::SetWorldMusicState()
{
	WorldState = EWorldState::WorldMusic; // TO EDIT

	//change state for players
	for (auto Character : PipouCharacters) 
	{
		if (Character && Character->StateMachine)
		{
			Character->StateMachine->ChangeState(EPipouCharacterStateID::Music); // Block Movement
		}
	}

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->CallMusicCamera(); // SetCameraMusic()
	
	UGlobalHUDSubsystem* HUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();
	if (!HUDSubsystem) return;
	
	// Display UI
	HUDSubsystem->DisplayResurrectionWidget();
	
	GetWorld()->GetSubsystem<UMusicWorldSubsystem>()->InitMusic(CurrentSkeleton);
}


