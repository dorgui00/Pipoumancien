// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PipouGameMode.generated.h"

class APipouCharacter;
class APlayerStart;

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API APipouGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
protected:
	UPROPERTY()
	TArray<APipouCharacter*> CharactersInMap;
	
private:
	void FindPlayerStartActorsInScene(TArray<APlayerStart*>& ResultActors);
	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);
	TSubclassOf<APipouCharacter> GetPipouCharacterFromInputType(EAutoReceiveInput::Type InputType) const;
	
};
