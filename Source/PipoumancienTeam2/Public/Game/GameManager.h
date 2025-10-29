// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Character/PipouCharacter.h"
#include "GameManager.generated.h"

class UInputAction;
/**
 * 
 */
struct F_Skeleton;

UENUM()
enum class EWorldState : uint8{
	None = 0,
	WorldFree = 1,
	WorldMusic = 2,
	WorldTransport = 3,
};

UCLASS()
class PIPOUMANCIENTEAM2_API AGameManager : public AActor
{
	GENERATED_BODY()
	
public :
	static AGameManager* Instance(UWorld* World);
	
	UPROPERTY()
	TArray<APipouCharacter*> PipouCharacters;
	
	void SetCharacters(APipouCharacter* Character);

	F_Skeleton* GetCurrentSkeleton();
	void SetCurrentSkeleton(F_Skeleton* Skeleton);

	// Music
	int NbNotesToCheck = 3;
	
	UPROPERTY()
	TArray<UInputAction*> InputPressed;
	
	void AddNote(UInputAction* InputAction);
	bool HasValidFirstNotes();
	void ResetInputsArray();

private :
	virtual void BeginPlay() override;

	EWorldState WorldState = EWorldState::WorldFree;
	
	F_Skeleton* CurrentSkeleton = nullptr;

	static AGameManager* MyInstance;

	void SetWorldMusicState();
	
};

