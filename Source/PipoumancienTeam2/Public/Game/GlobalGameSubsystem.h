// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/PipouCharacter.h"
#include "GlobalGameSubsystem.generated.h"

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
	Menus = 4,
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

	// Current Skeleton
	ASkeletonController* GetCurrentSkeleton() const;
	void SetCurrentSkeleton(ASkeletonController* Skeleton); // TO EDIT ? Switch to private ?

	// Music
	// Skeleton Interaction
	int SkeletonNotesToCheck = 3; // check 3 skeleton notes to trigger main music meca
	
	UPROPERTY()
	TArray<UInputAction*> InputPressed;

	void AddNoteForSkeletonInteraction(UInputAction* InputAction); // add input
	bool HasValidFirstNotes();
	void ResetInputsArray();

	void CheckIfPlayersOverlapSameSkeleton();

	//DEBUG
	void SetWorldMusicState();
	EWorldState GetWorldState() const;
	
private :

	// World State
	EWorldState WorldState = EWorldState::WorldFree; // TO EDIT

	//DEBUG
	//void SetWorldMusicState();
	
	// Skeleton
	UPROPERTY()
	ASkeletonController* CurrentSkeleton = nullptr;
};
