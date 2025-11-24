// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elements/Common/TypedElementCommonTypes.h"
#include "GameFramework/GameModeBase.h"
#include "PipouGameMode.generated.h"

class UInputSoundData;
class UCameraComponent;
class UInputMappingContext;
class UPipouCharacterInputData;
class APipouCharacter;
class APlayerStart;

UCLASS()
class PIPOUMANCIENTEAM2_API APipouGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	//Cameras
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;
	
protected:
	// players
	UPROPERTY()
	TArray<APipouCharacter*> CharactersInMap;
	
private:
	// Inputs
	UPipouCharacterInputData* LoadInputDataFromConfig();
	UInputMappingContext* LoadInputMappingContextFromConfig();

	// World Sounds
	UInputSoundData* LoadInputSoundDataFromConfig();
	void InitWorldSoundsMap() const ;
	
	// Cameras
	void GetCamera();

	// Players
	UPROPERTY()
	TArray<APlayerStart*> PlayerStartsPoint;
	void CreateAndInitPlayers();
	
	void FindPlayerStartActorsInScene(TArray<APlayerStart*>& ResultActors);
	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);
	TSubclassOf<APipouCharacter> GetPipouCharacterFromInputType(EAutoReceiveInput::Type InputType) const;

};
