// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/VillagePathManager.h"
#include "Tools/PathManager.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"

AVillagePathManager::AVillagePathManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AVillagePathManager::BeginPlay()
{
    Super::BeginPlay();

    if (!PathManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("A skeleton is missing a path"));
    }
}

USplineComponent* AVillagePathManager::GetSplineForSkeleton(AActor* Skeleton) const
{
    if (!Skeleton || !PathManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing Skeleton or PathManager."));
        return nullptr;
    }

    AActor* EndActor = nullptr;

    for (const FSkeletonVillageRoute& Route : SkeletonRoutes)
    {
        if (Route.Skeleton == Skeleton)
        {
            EndActor = Route.EndPoint;
            break;
        }
    }

    if (!EndActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: No route configured for skeleton %s"),
            *GetNameSafe(Skeleton));
        return nullptr;
    }

    const int32 StartIdx = PathManager->GetClosestStartIndex(Skeleton->GetActorLocation());
    if (StartIdx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: No valid StartIndex for skeleton %s"),
            *GetNameSafe(Skeleton));
        return nullptr;
    }

    int32 EndIdx = INDEX_NONE;
    for (int32 i = 0; i < PathManager->EndPoints.Num(); ++i)
    {
        if (PathManager->EndPoints[i] == EndActor)
        {
            EndIdx = i;
            break;
        }
    }

    if (EndIdx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: EndPoint %s is not in PathManager->EndPoints"),
            *GetNameSafe(EndActor));
        return nullptr;
    }

    return PathManager->GetSplineForIndices(StartIdx, EndIdx);
}
