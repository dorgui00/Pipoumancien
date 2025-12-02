// Fill out your copyright notice in the Description page of Project Settings.

#include "PNJ/AC_SetAnimations.h"
#include "Components/ActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimSequence.h"
#include "Animation/AnimationAsset.h"
#include "PNJ/AC_SkeletonFollower.h"

UAC_SetAnimations::UAC_SetAnimations()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_SetAnimations::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (!Owner) return;

    TargetMesh = Cast<USkeletalMeshComponent>(TargetMeshRef.GetComponent(Owner));

    if (!TargetMesh)
    {
        return;
    }

    SkeletonFollower = Owner->FindComponentByClass<UAC_SkeletonFollower>();

    LastLocation = Owner->GetActorLocation();
    bHasLastLocation = true;

    if (IdleAnimation)
    {
        TargetMesh->PlayAnimation(IdleAnimation, true);
    }
}


void UAC_SetAnimations::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner || !TargetMesh) return;

    float Speed = 0.f;
    const FVector CurrentLocation = Owner->GetActorLocation();

    if (bHasLastLocation && DeltaTime > KINDA_SMALL_NUMBER)
    {
        const float DistanceMoved = FVector::Dist(CurrentLocation, LastLocation);
        Speed = DistanceMoved / DeltaTime;
    }

    LastLocation = CurrentLocation;
    bHasLastLocation = true;

    const bool bIsMoving = Speed > WalkSpeedThreshold;

    const bool bIsFollowing =
        (SkeletonFollower && SkeletonFollower->IsStartFollowing());

    if (bIsMoving != bWasMoving || bIsFollowing != bWasFollowing)
    {
        if (bIsFollowing && bIsMoving) //moving & following
        {
            if (WalkAnimation)
            {
                PlayWalk();
            }
        }
        else if (bIsFollowing && !bIsMoving) //not moving & following
        {
            if (WaitAnimation)
            {
                PlayWait();
            }
        }
        else if (!bIsFollowing && !bIsMoving) //not moving & not following
        {
            if (IdleAnimation)
            {
                PlayIdle();
            }
        }

        bWasMoving = bIsMoving;
        bWasFollowing = bIsFollowing;
    }
}


#pragma region Animation Calls

void UAC_SetAnimations::SetReference(FComponentReference TargetedMesh)
{
    TargetMeshRef = TargetedMesh;
}

void UAC_SetAnimations::SetAnimations(
    UAnimationAsset* IdleAnim,
    UAnimationAsset* WalkAnim,
    UAnimationAsset* WaitAnim)
{
    IdleAnimation = IdleAnim;
    WalkAnimation = WalkAnim;
    WaitAnimation = WaitAnim;
}

void UAC_SetAnimations::PlayIdle()
{
	if (TargetMesh && IdleAnimation)
	{
		TargetMesh->PlayAnimation(IdleAnimation, true);
	}
}

void UAC_SetAnimations::PlayWalk()
{
	if (TargetMesh && WalkAnimation)
	{
		TargetMesh->PlayAnimation(WalkAnimation, true);
	}
}

void UAC_SetAnimations::PlayWait()
{
	if (TargetMesh && WaitAnimation)
	{
		TargetMesh->PlayAnimation(WaitAnimation, true);
	}
}

#pragma endregion

