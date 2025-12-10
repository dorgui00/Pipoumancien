// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PipouCharacter.h"
#include "GlobalGameSubsystem.generated.h"

class ABird;
class UGlobalHUDSubsystem;
class UInputAction;
/**
 * 
 */
struct F_Skeleton;

// UENUM()
enum class EWorldState : uint8{
	None = 0,
	WorldFree = 1,
	WorldMusic = 2,
	WorldTransport = 3,
	WorldDialogue = 4,
	Menus = 5,
};

UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalGameSubsystem : public UGameInstanceSubsystem //, public FTickableGameObject
{
	GENERATED_BODY()

public :
	// Override
	//virtual void Tick(float DeltaTime) override;
	
	// Characters
	UPROPERTY()
	TArray<APipouCharacter*> PipouCharacters;

	void SetCharacters(APipouCharacter* Character);

	// Skeleton
	ASkeletonController* GetCurrentSkeleton() const;
	void SetCurrentSkeleton(ASkeletonController* Skeleton); // TO EDIT ? Switch to private ?

	// Bird
	void SetBird(ABird* InBird);
	ABird* GetBird();
	
	// --- MUSIC ---
	// Skeleton Interaction
	int SkeletonNotesToCheck = 3; // check 3 skeleton notes to trigger main music meca
	
	UPROPERTY()
	TArray<UInputAction*> InputPressed;

	void AddNoteForSkeletonInteraction(UInputAction* InputAction); // add input
	bool HasValidFirstNotes();
	void ResetInputsArray();

	bool PlayersOverlapSameSkeleton();
	void CancelOverlapSameSkeleton(); // no longer overlap same skeleton

	//melody
	void SetLostMelody();
	
	// State
	EWorldState GetWorldState() const;
	
	void SetWorldMusicState();

	void SetWorldTransportState();

	void SetWorldFreeState();

	void SetWorldDialogueState(APipouCharacter* Interactor, ASkeletonController* Speaker);
	
	void RimouveWidget();
	
private :

	// World State
	EWorldState WorldState = EWorldState::WorldFree; // TO EDIT
	
	// Skeleton
	UPROPERTY()
	ASkeletonController* CurrentSkeleton = nullptr;

	// Bird
	UPROPERTY()
	TObjectPtr<ABird> Bird = nullptr;
	

	// ---- UTILITIES ----

	// to init (not yet done)
	// UPROPERTY()
	// UGlobalHUDSubsystem* GlobalHUDSubsystem;
	
};
