// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PipouCharacterStateID.h"
#include "PipouCharacterState.generated.h"

class UInputAction;
struct FInputActionValue;
class APipouCharacter;
class UPipouCharacterStateMachine;
enum class EPipouCharacterStateID : uint8;
struct F_Skeleton;

UENUM()
enum class EPipouCharacterRoles : uint8
{
	None,
	Musician,
	Conductor,
};

UCLASS(Abstract)
class PIPOUMANCIENTEAM2_API UPipouCharacterState : public UActorComponent
{
	GENERATED_BODY()

public:
	UPipouCharacterState();

	// Manage State
	virtual EPipouCharacterStateID GetStateID();
	virtual void StateInit(UPipouCharacterStateMachine* InStateMachine);
	virtual void StateEnter(EPipouCharacterStateID PreviousStateID);
	virtual void StateTick(float Deltatime);
	virtual void StateExit(EPipouCharacterStateID NextStateID);

protected:
	UPROPERTY()
	TObjectPtr<APipouCharacter> Character;

	UPROPERTY()
	TObjectPtr<UPipouCharacterStateMachine> StateMachine;

	// NE PEUX PAS AVOIR UN UPROPERTY
	F_Skeleton* Skeleton = nullptr;

	UFUNCTION()
	virtual void OnCharacterPressedInput(UInputAction* InputAction, FInputActionValue InputActionValue);
};
