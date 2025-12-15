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


void UGlobalGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	Init();
}

void UGlobalGameSubsystem::Init()
{
	GlobalHUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();
	if (!GlobalHUDSubsystem)
		UE_LOG(LogTemp, Error, TEXT("Global HUD Subsystem is null"))
}

// void UGlobalGameSubsystem::Tick(float DeltaTime)
// {
// }

// Called In HUD
void UGlobalGameSubsystem::SetHUD(TObjectPtr<UGlobalHUDSubsystem> HUDSubsystem)
{
	GlobalHUDSubsystem =  HUDSubsystem;
}


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
	
	GlobalHUDSubsystem->InitBirdWidget(Bird);
}

ABird* UGlobalGameSubsystem::GetBird()
{
	return Bird;
}


// Music
void UGlobalGameSubsystem::AddNoteForSkeletonInteraction(UInputAction* InputAction)
{

	// TO EDIT
	//// First Note
	// if (InputPressed.Num() == 0)
	// {
	// 	GlobalHUDSubsystemIn->CallSkeletonInteractionWidget();
	// }
	
if (IsTouch)
{
	// Add Notes
	InputPressed.Add(InputAction);
	GlobalHUDSubsystem->DisplayNotesForSkeletonInteraction(InputAction);
	
	if (InputPressed.Num() >= SkeletonNotesToCheck)
	{
		IsTouch = false;
		if (HasValidFirstNotes())
		{
			//GlobalHUDSubsystemIn->ValideWidget();
			//FTimerHandle TimerHandle;
			//GetWorld()->GetTimerManager().SetTimer(TimerHandle, this,
			//&UGlobalGameSubsystem::SetWorldMusicState,
			//3.f, false);
			//SetWorldMusicState();
			//Bird->SetWidgetINVisible();

			GlobalHUDSubsystem->ValideWidget();
			SetWorldMusicState();
			ResetInputsArray();

			FTimerHandle TimerHandle;
			 GetWorld()->GetTimerManager().SetTimer(
			 	TimerHandle,
			 	[this]()
			 	{
			 		//Bird->SetWidgetINVisible();
					IsTouch = true;
			 	},
			 	2.f,
			 	false
			 );
		}
		else
		{

			FTimerHandle DelayHandle;
			GetWorld()->GetTimerManager().SetTimer(
				DelayHandle,
				FTimerDelegate::CreateLambda([this]()
				{
					// 1️⃣ FolseWidget() après 2 secondes
					GlobalHUDSubsystem->FolseWidget();

					// 2️⃣ Ensuite le reste du code
					ResetInputsArray();

					

					GlobalHUDSubsystem->RemoveBirdWidget();
					

				}),
				1.0f,  // délai AVANT FolseWidget
				false
				);
			
			FTimerHandle delayHandle;
		GetWorld()->GetTimerManager().SetTimer(
		   		delayHandle,
		   		FTimerDelegate::CreateLambda([this]()
			{
		   			if (Bird)
		   			{
						   Bird->SetMyNotes();
		   			}
					IsTouch = true;
				}),
				4.5f,  // délai AVANT FolseWidget
				false
				);
		}
	
}

		// --- RESET ---
		// Reset 3 Notes
		//ResetInputsArray();
		
		
		// Reset UI
		//FTimerHandle TimerHandle;
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle, GlobalHUDSubsystemIn,
		//	&UGlobalHUDSubsystem::RimouveWidget(),
		//	1.f, false);

		//GetWorld()->GetTimerManager().SetTimer(TimerHandle,FTimerDelegate::CreateLambda([this]()
	//{
		// Code exécuté après 5s
		//Bird->SetMyNotes();
	//}),
	//2.0f,
	//false
//);
		//GlobalHUDSubsystemIn->RemoveBirdWidget();
		
	}
}



bool UGlobalGameSubsystem::HasValidFirstNotes()
{
	for (int i = 0; i < SkeletonNotesToCheck; ++i)
	{
		if (CurrentSkeleton->MySkeleton->Notes[i].InputAction != InputPressed[i])
		{
			// UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes raté"));
			// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Enchainement de 3 notes raté")), true, FVector2D(2, 2));
			
			return false;
		}
	}

	// UE_LOG(LogTemp, Display, TEXT("Enchainement de 3 notes réussi"));
	// GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, FString::Printf(TEXT("Enchainement de 3 notes réussi")), true, FVector2D(2, 2));
	
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

	// For music interaction
	if (CurrentSkeletonIn->GetState()!=ESkeletonState::Dead) return false;

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
	ResetInputsArray();
	// FEEDBACKS
	Bird->SetWidgetVisibility(false);
}


void UGlobalGameSubsystem::SetLostMelody()
{
	GlobalHUDSubsystem->RemovePartitionFinish();

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

	// VISUELS
	Bird->SetWidgetVisibility(false);
	
	// Display UI
	GlobalHUDSubsystem->DisplayResurrectionWidget();
	
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






