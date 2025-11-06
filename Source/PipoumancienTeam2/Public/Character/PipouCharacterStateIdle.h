// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PipouCharacterState.h"
#include "PipouCharacterStateIdle.generated.h"

enum class EPipouCharacterStateID : uint8;
class UAnimationAsset;

UCLASS(ClassGroup=(PipouCharacterState), meta=(BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UPipouCharacterStateIdle : public UPipouCharacterState
{
	GENERATED_BODY()

public:
	virtual EPipouCharacterStateID GetStateID() override;
	virtual void StateEnter(EPipouCharacterStateID PreviousStateID) override;
	virtual void StateTick(float Deltatime) override;
	virtual void StateExit(EPipouCharacterStateID NextStateID) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimationAsset* IdleAnim;

	// Music
	virtual void OnCharacterPressedNote(UInputAction* InputAction) override;
};
