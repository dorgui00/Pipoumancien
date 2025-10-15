// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PipouCharacterStateMachine.generated.h"

class APipouCharacter;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UPipouCharacterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void Init(APipouCharacter* InPipouCharacter);
	APipouCharacter* GetPipouCharacter() const;

private:
	UPROPERTY()
	TObjectPtr<APipouCharacter> Character;
};
