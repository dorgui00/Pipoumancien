// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PipouCharacter.h"
#include "PipouCharacterState.h"
#include "PipouCharacterStateMusic.generated.h"

class AMusicManager;
struct FInputActionValue;
class UInputAction;
class ASkeletonController;

UCLASS(ClassGroup=(PipouCharacterState), meta=(BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UPipouCharacterStateMusic : public UPipouCharacterState
{
	GENERATED_BODY()

public:
	virtual EPipouCharacterStateID GetStateID() override;
	virtual void StateEnter(EPipouCharacterStateID PreviousStateID) override;
	virtual void StateTick(float Deltatime) override;
	virtual void StateExit(EPipouCharacterStateID NextStateID) override;

	// Music
	UPROPERTY()
	EPipouCharacterRoles CurrentRole;

	void InitRoles();
	void InitSkeletons();
	void InitInputPitch();
	void SetMusicManager();

	UPROPERTY()
	float PitchTolerance = 0.1f;

	UPROPERTY()
	UInputAction* InputPitch;

	UPROPERTY()
	AMusicManager* MusicManager;
	
	virtual void OnCharacterPressedNote(UInputAction* InputAction) override;
	virtual void OnCharacterPitch(FInputActionValue InputActionValue) override;
};
