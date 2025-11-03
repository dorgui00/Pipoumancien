// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

#include "EngineUtils.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Kismet/GameplayStatics.h"
#include "Music/MusicManager.h"


AGameManager* AGameManager::MyInstance ;

AGameManager* AGameManager::Instance(UWorld* World)
{
	if (IsValid(MyInstance))
		return MyInstance;

	for (TActorIterator<AGameManager> It(World); It; ++It)
	{
		MyInstance = *It;
		break;
	}

	if (!MyInstance)
	{
		MyInstance = World->SpawnActor<AGameManager>(AGameManager::StaticClass());
	}

	return MyInstance;
}

// to call in init pipou chara
void AGameManager::SetCharacters(APipouCharacter* Character)
{
	PipouCharacters.Add(Character);
}

F_Skeleton* AGameManager::GetCurrentSkeleton()
{
	return CurrentSkeleton;
}

void AGameManager::SetCurrentSkeleton(F_Skeleton* Skeleton)
{
	CurrentSkeleton = Skeleton;
}

// Music
void AGameManager::AddNote(UInputAction* InputAction)
{
	InputPressed.Add(InputAction);

	if (InputPressed.Num() >= NbNotesToCheck)
	{
		if (HasValidFirstNotes()) 
			SetWorldMusicState();
		
		ResetInputsArray();
	}
}

bool AGameManager::HasValidFirstNotes()
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

void AGameManager::ResetInputsArray()
{
	InputPressed.Empty();
}

void AGameManager::BeginPlay()
{
	Super::BeginPlay();
}

void AGameManager::SetWorldMusicState()
{
	WorldState = EWorldState::WorldMusic;

	//change state for players
	for (auto Character : PipouCharacters) 
	{
		if (Character && Character->StateMachine)
		{
			Character->StateMachine->ChangeState(EPipouCharacterStateID::Music);
		}
	}
	
	//// Pailletas
	// SetAllMusicBehavior()
	// BlockMovement()
	// SetCameraMusic()
	DisplayResurrectionUI();
	AMusicManager::Instance(GetWorld())->InitMusicBySkeleton(CurrentSkeleton);
}

void AGameManager::DisplayResurrectionUI()
{
	APlayerController* PlayerController = PipouCharacters[0]->GetController<APlayerController>();
	if (PlayerController == nullptr) return;

	APipouHUD* PipoouHUD = PipouCharacters[0]->GetHUD();
	if (PipoouHUD == nullptr) return;
	
	PipoouHUD->AddWBPResurrection(PipouCharacters[0]->GetController<APlayerController>());

	for (F_Note Note : CurrentSkeleton->Notes)
	{
		PipoouHUD->AddWbpSlotInstance(PlayerController, Note.Pitch, Note.InputAction);
	}
}

void AGameManager::RemoveResurrectionUI()
{
	APlayerController* PlayerController = PipouCharacters[0]->GetController<APlayerController>();
	if (PlayerController == nullptr) return;

	APipouHUD* PipoouHUD = PipouCharacters[0]->GetHUD();
	if (PipoouHUD == nullptr) return;
	
	PipoouHUD->RemoveResurrection();
}
