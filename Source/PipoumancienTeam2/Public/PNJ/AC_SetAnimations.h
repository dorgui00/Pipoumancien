// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PNJ/AC_SkeletonFollower.h"
#include "Components/ActorComponent.h"
#include "AC_SetAnimations.generated.h"

class USkeletalMeshComponent;
class UAnimSequence;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UAC_SetAnimations : public UActorComponent
{
	GENERATED_BODY()

public:
	UAC_SetAnimations();

protected:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, Category = "Animations")
	FComponentReference TargetMeshRef;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* IdleAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WalkAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WaitAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	float WalkSpeedThreshold = 5.f;

private:

	UPROPERTY()
	UAC_SkeletonFollower* SkeletonFollower = nullptr;

	UPROPERTY()
	USkeletalMeshComponent* TargetMesh = nullptr;

	FVector LastLocation = FVector::ZeroVector;
	bool bHasLastLocation = false;

	bool bWasMoving = false;
	bool bWasFollowing = false;

	void PlayIdle();
	void PlayWalk();
	void PlayWait();

};
