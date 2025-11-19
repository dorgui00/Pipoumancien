// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GlobalGameSubsystem.h"

#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Character/PipouCharacterStateID.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Music/MusicWorldSubsystem.h"
#include "PNJ/SkeletonController.h"
#include "UI/GlobalHUDSubsystem.h"

// void UGlobalGameSubsystem::Tick(float DeltaTime)
// {
// }

// to call in init pipou chara
void UGlobalGameSubsystem::SetCharacters(APipouCharacter* Character)
{
	PipouCharacters.Add(Character);
}

ASkeletonController* UGlobalGameSubsystem::GetCurrentSkeleton() const
{
	return CurrentSkeleton;
}

void UGlobalGameSubsystem::SetCurrentSkeleton(ASkeletonController* Skeleton)
{
	CurrentSkeleton = Skeleton;
}

// Music
void UGlobalGameSubsystem::AddNoteForSkeletonInteraction(UInputAction* InputAction)
{
	InputPressed.Add(InputAction);

	if (InputPressed.Num() >= SkeletonNotesToCheck)
	{
		if (HasValidFirstNotes()) 
			SetWorldMusicState();
		
		ResetInputsArray();
	}
}

bool UGlobalGameSubsystem::HasValidFirstNotes()
{
	for (int i = 0; i < SkeletonNotesToCheck; ++i)
	{
		if (CurrentSkeleton->MySkeleton->Notes[i].InputAction != InputPressed[i])
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

// Check If Anybody Still Overlaps The Current Skeleton
void UGlobalGameSubsystem::CheckIfPlayersOverlapSameSkeleton()
{
	if (PipouCharacters.Num() == 0 )
		UE_LOG(LogTemp, Error, TEXT("No players found"));
	
	// set current at the overlap skeleton of the first player
	ASkeletonController* CurrentSkeletonIn = PipouCharacters[0]->OverlapSkeleton;
	
	// first player doesn't overlap a skeleton => cancel checking
	if (!CurrentSkeletonIn) return;

	for (auto Character : PipouCharacters)
	{
		// doesn't overlap the same skel
		if (CurrentSkeletonIn !=  Character->OverlapSkeleton)
			return;
	}
	
	//Players Overlap the same skel
	UE_LOG(LogTemp, Display, TEXT("Players are Overlapping the same skeleton"));
	SetCurrentSkeleton(CurrentSkeletonIn);
}

EWorldState UGlobalGameSubsystem::GetWorldState() const
{
	return WorldState;
}

void UGlobalGameSubsystem::SetWorldMusicState()
{
	WorldState = EWorldState::WorldMusic; 
	
	//change state for players
	for (auto Character : PipouCharacters) 
	{
		if (Character && Character->StateMachine)
		{
			Character->StateMachine->ChangeState(EPipouCharacterStateID::Music); // Block Movement
		}
	}

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetMusicCamera() ;
	
	UGlobalHUDSubsystem* HUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();
	if (!HUDSubsystem) return;
	
	// Display UI
	HUDSubsystem->DisplayResurrectionWidget();
	
	GetWorld()->GetSubsystem<UMusicWorldSubsystem>()->InitMusic(CurrentSkeleton);
}

void UGlobalGameSubsystem::SetWorldTransportState()
{
	UE_LOG(LogTemp, Display, TEXT("World State Transport"));
	
	// WORLD STATE
	WorldState = EWorldState::WorldTransport;

	// Pipou IDLE
	for (APipouCharacter* PipouCharacter : PipouCharacters)
	{
		PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Idle);
	}

	// Add Follow Component
	GetCurrentSkeleton()->SetSkeletonForTransport();
}



