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
	AGameManager();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	static AGameManager* Instance(UWorld* World);

	UPROPERTY()
	APipouHUD* PipouHUD;
	
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

	/// TO EDIT
	void RemoveResurrectionUI();
	
private :
	EWorldState WorldState = EWorldState::WorldFree;
	
	F_Skeleton* CurrentSkeleton = nullptr;

	static AGameManager* MyInstance;

	void SetWorldMusicState();

	void DisplayResurrectionUI();

	USlot* NotePanel;
	
};

