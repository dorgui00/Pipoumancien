// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Elements/Common/TypedElementCommonTypes.h"
#include "GameFramework/GameModeBase.h"
#include "PipouGameMode.generated.h"

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

	UPROPERTY()
	TObjectPtr<AActor> CameraActor;
	
protected:
	UPROPERTY()
	TArray<APipouCharacter*> CharactersInMap;
	
private:
	UPipouCharacterInputData* LoadInputDataFromConfig();
	UInputMappingContext* LoadInputMappingContextFromConfig();
	
	void FindPlayerStartActorsInScene(TArray<APlayerStart*>& ResultActors);
	void SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints);
	TSubclassOf<APipouCharacter> GetPipouCharacterFromInputType(EAutoReceiveInput::Type InputType) const;

	void GetCameraByTag(const FName& CameraTag);

	void CreateAndInitPlayers();

};
