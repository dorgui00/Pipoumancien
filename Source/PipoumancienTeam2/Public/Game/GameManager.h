// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Character/PipouCharacter.h"
#include "GameManager.generated.h"

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
	static AGameManager* Instance();
	
	F_Skeleton* CurrentSkeleton = nullptr;

	UPROPERTY()
	TArray<APipouCharacter*> PipouCharacters;
	
	void AddPipouCharacter(APipouCharacter* Character);

protected :
	virtual void BeginPlay() override;

	EWorldState WorldState = EWorldState::WorldFree;
	
private :
	static AGameManager* MyInstance;

	void SetWorldMusicState(F_Skeleton* Skeleton);
	
};

