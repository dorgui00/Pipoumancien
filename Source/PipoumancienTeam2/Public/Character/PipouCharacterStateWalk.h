// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PipouCharacterState.h"
#include "PipouCharacterStateWalk.generated.h"

enum class EPipouCharacterStateID : uint8;
class UAnimationAsset;
class USoundBase;
class UAudioComponent;

UCLASS(ClassGroup=(PipouCharacterState), meta=(BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UPipouCharacterStateWalk : public UPipouCharacterState
{
	GENERATED_BODY()

public:
	// ---- STATE DEFAULT FUNCTIONS ----
	virtual EPipouCharacterStateID GetStateID() override;
	virtual void StateEnter(EPipouCharacterStateID PreviousStateID) override;
	virtual void StateTick(float Deltatime) override;
	virtual void StateExit(EPipouCharacterStateID NextStateID) override;

	// ---- ANIMATIONS ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Animation")
	UAnimationAsset* WalkAnim;

	// ---- MOVEMENTS ----
	UPROPERTY()
	FVector MoveDir;
	
	UPROPERTY(EditAnywhere)
	float MoveSpeed;

	UFUNCTION(BlueprintCallable)
	FVector GetMoveDirection() const;

	//AUDIO

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	USoundBase* WalkSoundBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio/IterateOnGround")
	USoundBase* GrassFootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio/IterateOnGround")
	USoundBase* DirtFootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio/IterateOnGround")
	USoundBase* TownRoadFootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio/IterateOnGround")
	USoundBase* SnowFootstepSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
	bool IterateOnGround = false;

	void PlayFootstepsSound(UPhysicalMaterial* PhysMat);

	// ----- //

	UPROPERTY(Transient)
	UAudioComponent* WalkLoopComponent;

private:

	//AUDIO
	float FootstepTimer = 0.f;
	float FootstepInterval = 1.f; // default step rate
	// --- //
};
