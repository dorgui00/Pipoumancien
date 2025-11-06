// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Character/PipouCharacter.h"
#include "GlobalGameSubsystem.generated.h"

class UInputAction;
/**
 * 
 */
struct F_Skeleton;

// UENUM()
// enum class EWorldState : uint8{
// 	None = 0,
// 	WorldFree = 1,
// 	WorldMusic = 2,
// 	WorldTransport = 3,
// 	Menus = 4,
// };

UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalGameSubsystem : public UGameInstanceSubsystem //, public FTickableGameObject
{
	GENERATED_BODY()

public :
	// Characters
	UPROPERTY()
	TArray<APipouCharacter*> PipouCharacters;

	void SetCharacters(APipouCharacter* Character);

	// Current Skeleton
	F_Skeleton* GetCurrentSkeleton() const;
	void SetCurrentSkeleton(F_Skeleton* Skeleton);

	// Music
	int NbNotesToCheck = 3;
	
	UPROPERTY()
	TArray<UInputAction*> InputPressed;
	
	void AddNote(UInputAction* InputAction); // add input
	bool HasValidFirstNotes();
	void ResetInputsArray();

	// UI
	UPROPERTY()
	APipouHUD* PipouHUD;

	UPROPERTY()
	USlot* NotePanel;
	
	void RemoveResurrectionUI(); // TO EDIT
	
private :

	// World State
	//EWorldState WorldState = EWorldState::WorldFree; // TO EDIT

	void SetWorldMusicState();
	
	// Skeleton
	F_Skeleton* CurrentSkeleton = nullptr;

	// UI
	void DisplayResurrectionUI();
};
