// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PipouCharacter.h"
#include "PipouCharacterState.h"
#include "PipouCharacterStateMusic.generated.h"

class UMusicWorldSubsystem;
class AMusicManager;
struct FInputActionValue;
class UInputAction;
class ASkeletonController;

UCLASS(ClassGroup=(PipouCharacterState), meta=(BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UPipouCharacterStateMusic : public UPipouCharacterState
{
	GENERATED_BODY()

public:
	// ---- DEFAULT STATE FUNCTIONS ----
	virtual EPipouCharacterStateID GetStateID() override;
	virtual void StateEnter(EPipouCharacterStateID PreviousStateID) override;
	virtual void StateTick(float Deltatime) override;
	virtual void StateExit(EPipouCharacterStateID NextStateID) override;

	// ---- MUSIC ----
	UPROPERTY()
	EPipouCharacterRoles CurrentRole;

	void InitRole();
	void InitSkeletons();
	void InitInputPitch();
	void SetMusicManager();

	UPROPERTY()
	UInputAction* InputPitch;

	UPROPERTY()
	float InitPitchSpeedValue = 1.f;
	
	UPROPERTY()
	UMusicWorldSubsystem* MusicWorldSubsystem;
	
	virtual void OnCharacterPressedNote(UInputAction* InputAction) override;
	virtual void OnCharacterPitch(FInputActionValue InputActionValue) override;
	virtual void OnCharacterPitchCompleted() override;

	// ---- ANIMATIONS ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimationAsset* MusicAnim;
};
