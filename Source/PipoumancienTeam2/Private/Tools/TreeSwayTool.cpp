// Fill out your copyright notice in the Description page of Project Settings.

#include "Tools/TreeSwayTool.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "Engine/StaticMeshActor.h"

ATreeSwayTool::ATreeSwayTool()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATreeSwayTool::BeginPlay()
{
	Super::BeginPlay();

	RandomStream.Initialize(RandomSeed);

	if (bScanOnBeginPlay)
	{
		ScanForTrees();
		RandomiseSwayTargets();
	}
}

void ATreeSwayTool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (SwayDuration <= 0.0f)
	{
		return;
	}

	const float Omega = 2.0f * PI / SwayDuration;

	for (FTreeSwayData& Data : Trees)
	{
		if (!IsValid(Data.Tree))
		{
			continue;
		}

		Data.ElapsedTime += DeltaTime;

		if (Data.ElapsedTime < Data.Delay)
		{
			Data.Tree->SetActorRotation(Data.BaseRotation);
			continue;
		}

		const float LocalTime = Data.ElapsedTime - Data.Delay;

		const float S = FMath::Sin(LocalTime * Omega);

		const FRotator SwayOffset(
			Data.TargetOffset.Pitch * S,
			Data.TargetOffset.Yaw * S,
			Data.TargetOffset.Roll * S
		);

		const FRotator NewRot = Data.BaseRotation + SwayOffset;
		Data.Tree->SetActorRotation(NewRot);
	}
}

void ATreeSwayTool::ScanForTrees()
{
	Trees.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FString Prefix = TreeNamePrefix;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		const FString Id = GetActorIdForMatching(Actor);
		if (!Id.StartsWith(Prefix))
		{
			continue;
		}

		if (USceneComponent* Root = Actor->GetRootComponent())
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				Root->Modify();
			}
#endif
			Root->SetMobility(EComponentMobility::Movable);
		}

		FTreeSwayData Data;
		Data.Tree = Actor;
		Data.BaseRotation = Actor->GetActorRotation();
		Data.TargetOffset = FRotator::ZeroRotator;
		Data.ElapsedTime = 0.0f;
		Data.Delay = 0.0f;
		Data.Direction = 1.0f;

		Trees.Add(Data);
	}
}

void ATreeSwayTool::RandomiseSwayTargets()
{
	RandomStream.Initialize(RandomSeed);

	for (FTreeSwayData& Data : Trees)
	{
		if (!IsValid(Data.Tree))
		{
			continue;
		}

		Data.BaseRotation = Data.Tree->GetActorRotation();
		Data.TargetOffset = GetRandomOffset();
		Data.Delay = GetRandomDelay();
		Data.ElapsedTime = 0.0f;
		Data.Direction = 1.0f;
	}
}

FRotator ATreeSwayTool::GetRandomOffset() const
{
	const float Pitch = RandomStream.FRandRange(RotationOffsetMin.Pitch, RotationOffsetMax.Pitch);
	const float Yaw = RandomStream.FRandRange(RotationOffsetMin.Yaw, RotationOffsetMax.Yaw);
	const float Roll = RandomStream.FRandRange(RotationOffsetMin.Roll, RotationOffsetMax.Roll);

	return FRotator(Pitch, Yaw, Roll);
}

float ATreeSwayTool::GetRandomDelay() const
{
	if (SwayDelayMax <= SwayDelayMin)
	{
		return SwayDelayMin;
	}

	return RandomStream.FRandRange(SwayDelayMin, SwayDelayMax);
}

FString ATreeSwayTool::GetActorIdForMatching(AActor* Actor) const
{
	if (!Actor)
	{
		return FString();
	}

#if WITH_EDITOR
	return Actor->GetActorLabel();
#else
	return Actor->GetName();
#endif
}