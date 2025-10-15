// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PipouCharacter.generated.h"

class UPipouCharacterStateMachine;

UCLASS()
class PIPOUMANCIENTEAM2_API APipouCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APipouCharacter();

	// State Machine
	void CreateStateMachine();
	void InitStateMachine();

protected:
	virtual void BeginPlay() override;

	// State Machine
	UPROPERTY()
	TObjectPtr<UPipouCharacterStateMachine> StateMachine;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
