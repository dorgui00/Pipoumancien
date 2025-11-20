// Fill out your copyright notice in the Description page of Project Settings.

#include "PNJ/AC_SetAnimations.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "PNJ/AC_SkeletonFollower.h"

UAC_SetAnimations::UAC_SetAnimations()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_SetAnimations::BeginPlay()
{
	Super::BeginPlay();

	if (TargetMesh && IdleAnim)
	{
		TargetMesh->PlayAnimation(IdleAnim, true);
	}
}

void UAC_SetAnimations::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner || !TargetMesh) return;

    const float Speed = Owner->GetVelocity().Size();
    const bool bIsMoving = Speed > WalkSpeedThreshold;

    const bool bIsFollowing =
        (SkeletonFollower && SkeletonFollower->IsStartFollowing());

    if (bIsMoving != bWasMoving || bIsFollowing != bWasFollowing)
    {
        if (bIsFollowing && bIsMoving) //moving & following
        {
            if (WalkAnim)
            {
                PlayWalk();
            }
        }
        else if (bIsFollowing && !bIsMoving) //not moving & following
        {
            if (WaitAnim)
            {
                PlayWait();
            }
        }
        else if (!bIsFollowing && !bIsMoving) //not moving & not following
        {
            if (IdleAnim)
            {
                PlayIdle();
            }
        }

        bWasMoving = bIsMoving;
        bWasFollowing = bIsFollowing;
    }
}


#pragma region Animation Calls

void UAC_SetAnimations::PlayIdle()
{
	if (TargetMesh && IdleAnim)
	{
		TargetMesh->PlayAnimation(IdleAnim, true);
	}
}

void UAC_SetAnimations::PlayWalk()
{
	if (TargetMesh && WalkAnim)
	{
		TargetMesh->PlayAnimation(WalkAnim, true);
	}
}

void UAC_SetAnimations::PlayWait()
{
	if (TargetMesh && WaitAnim)
	{
		TargetMesh->PlayAnimation(WaitAnim, true);
	}
}

#pragma endregion

