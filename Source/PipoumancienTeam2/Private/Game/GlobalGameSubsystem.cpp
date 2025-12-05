// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GlobalGameSubsystem.h"

#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Character/PipouCharacterStateID.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Music/MusicWorldSubsystem.h"
#include "PNJ/Bird.h"
#include "PNJ/SkeletonController.h"
#include "Tools/Slider/NoteMapping.h"
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

void UGlobalGameSubsystem::SetBird(ABird* InBird)
{
	Bird = InBird;
}


// Music
void UGlobalGameSubsystem::AddNoteForSkeletonInteraction(UInputAction* InputAction)
{
	// HUD 
	UGlobalHUDSubsystem* GlobalHUDSubsystemIn = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();

	// First Note
	if (InputPressed.Num() == 0)
	{
		GlobalHUDSubsystemIn->CallSkeletonInteractionWidget();
	}

	// Add Notes
	InputPressed.Add(InputAction);
	GlobalHUDSubsystemIn->DisplayNotesForSkeletonInteraction(InputAction);

	// Compare notes
	if (InputPressed.Num() >= SkeletonNotesToCheck)
	{
		if (HasValidFirstNotes())
		{
			SetWorldMusicState();
		}

		// Reset 3 Notes
		ResetInputsArray();
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, GlobalHUDSubsystemIn,
			&UGlobalHUDSubsystem::ResetSkeletonInteractionWidget,
			1.f, false);
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
bool UGlobalGameSubsystem::PlayersOverlapSameSkeleton()
{
	// secu
	if (PipouCharacters.Num() == 0 )
		UE_LOG(LogTemp, Error, TEXT("No players found"));
	
	// set current at the overlap skeleton of the first player
	ASkeletonController* CurrentSkeletonIn = PipouCharacters[0]->OverlapSkeleton;
	
	// first player doesn't overlap a skeleton => cancel checking
	if (!CurrentSkeletonIn) return false;

	for (auto Character : PipouCharacters)
	{
		// doesn't overlap the same skel
		if (CurrentSkeletonIn !=  Character->OverlapSkeleton)
			return false;
	}
	
	// ---- PLAYERS OVERLAP SAME SKELETON ----
	UE_LOG(LogTemp, Display, TEXT("Players are Overlapping the same skeleton"));
	SetCurrentSkeleton(CurrentSkeletonIn);

	// feedbacks
	Bird->SetWidgetVisibility(true);
	
	return true;
}

void UGlobalGameSubsystem::CancelOverlapSameSkeleton()
{
	SetCurrentSkeleton(nullptr);

	// FEEDBACKS
	Bird->SetWidgetVisibility(false);
}

void UGlobalGameSubsystem::SetLostMelody()
{
	UGlobalHUDSubsystem* HUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();
	HUDSubsystem->RemovePartitionFinish();

	SetWorldFreeState();

	// VISUELS
	Bird->SetWidgetVisibility(true);
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

	// CAMERA
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetMusicCamera() ;
	
	UGlobalHUDSubsystem* HUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();
	if (!HUDSubsystem) return;

	// VISUELS
	Bird->SetWidgetVisibility(false);
	
	// Display UI
	HUDSubsystem->DisplayResurrectionWidget();
	
	GetWorld()->GetSubsystem<UMusicWorldSubsystem>()->InitMusic(CurrentSkeleton);
}

void UGlobalGameSubsystem::SetWorldTransportState()
{
	UE_LOG(LogTemp, Display, TEXT("World State Transport"));
	
	// WORLD STATE
	WorldState = EWorldState::WorldTransport;

	// Add Follow Component
	GetCurrentSkeleton()->SetSkeletonForTransport();

	// Camera
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetGlobalCamera();
}

// called when current skeleton reached village
void UGlobalGameSubsystem::SetWorldFreeState()
{
	UE_LOG(LogTemp, Display, TEXT("World State Free"));
	
	// WORLD STATE
	WorldState = EWorldState::WorldFree;
	
	// CAMERA
	UCameraWorldSubsystem* CameraWorldSubsystem = GetWorld()->GetSubsystem<UCameraWorldSubsystem>();
	if (CameraWorldSubsystem->GetState() == ECameraState::GlobalCamera) return;

	CameraWorldSubsystem->SetGlobalCamera(); // set characters in idle at the end of the lerp
	
}

void UGlobalGameSubsystem::SetWorldDialogueState(APipouCharacter* Interactor, ASkeletonController* Speaker)
{
	UE_LOG(LogTemp, Display, TEXT("World State Dialogue"));
	
	// WORLD STATE
	WorldState = EWorldState::WorldDialogue;

	// Pipou Dialogue
	for (APipouCharacter* PipouCharacter : PipouCharacters)
	{
		PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Dialogue);
	}

	// CAMERA
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->SetDialogueCamera(Interactor,Speaker) ;

	// to edit (call in finish camera)
	//Open dialogue
	Speaker->OpenDialogue();
}



