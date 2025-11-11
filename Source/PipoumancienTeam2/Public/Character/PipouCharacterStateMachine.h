// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PipouCharacterStateMachine.generated.h"

class UPipouCharacterStateMachine;
class APipouCharacter;
class UPipouCharacterState;
enum class EPipouCharacterStateID : uint8;

UCLASS()
class PIPOUMANCIENTEAM2_API UPipouCharacterStateMachine : public UObject
{
	GENERATED_BODY()

public:
	void Init(APipouCharacter* InPipouCharacter);
	void Tick(float DeltaTime);
	
	APipouCharacter* GetPipouCharacter() const;
	UPipouCharacterState* GetState(EPipouCharacterStateID StateID);
	UPipouCharacterState* GetCurrentState(); // TO SUPP ?
	void ChangeState(EPipouCharacterStateID NextStateID);
	
private:
	UPROPERTY()
	TObjectPtr<APipouCharacter> Character;

	UPROPERTY()
	TArray<UPipouCharacterState*> AllStates;

	UPROPERTY()
	EPipouCharacterStateID CurrentStateID;

	UPROPERTY()
	TObjectPtr<UPipouCharacterState> CurrentState;
	
	void FindStates();
	void InitStates();
};
