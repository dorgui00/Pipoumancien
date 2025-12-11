// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FSkeletonVisuals.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FSkeletonVisuals 
{
	GENERATED_USTRUCT_BODY()
	
	// MESH
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<USkeletalMesh> SkeletonMesh;

	// ANIMS
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimationAsset> WakeAnim;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimationAsset> IdleAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimationAsset> WalkAnim;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAnimationAsset> WaitAnim;

	
};