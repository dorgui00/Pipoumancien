// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PathManager.h"
#include "Tools/SplinePathGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Tools/SplinePathGenerator.h"

APathManager::APathManager()
{
    PrimaryActorTick.bCanEverTick = false;
    SetActorTickEnabled(false);

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
}

void APathManager::BeginPlay()
{
    Super::BeginPlay();

    if (!SourceGenerator)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASplinePathGenerator::StaticClass(), Found);
        if (Found.Num() > 0)
        {
            SourceGenerator = Cast<ASplinePathGenerator>(Found[0]);
        }
    }

    BuildFromGenerator();
}

void APathManager::BuildFromGenerator()
{
    ClearRuntimeSplines();

    if (!SourceGenerator || !GetWorld())
    {
        return;
    }

    StartPoints = SourceGenerator->StartPoints;
    EndPoints = SourceGenerator->EndPoints;

    StartPoints.RemoveAll([](AActor* A) { return A == nullptr; });
    EndPoints.RemoveAll([](AActor* A) { return A == nullptr; });

    if (StartPoints.Num() == 0 || EndPoints.Num() == 0)
    {
        return;
    }

    SplineMatrix = SourceGenerator->GeneratedSplines;

    const int32 ExpectedNum = StartPoints.Num() * EndPoints.Num();
}

void APathManager::ClearRuntimeSplines()
{
    SplineMatrix.Empty();
}

AActor* APathManager::GetEndPoint(int32 EndIdx) const
{
    return EndPoints.IsValidIndex(EndIdx) ? EndPoints[EndIdx] : nullptr;
}

USplineComponent* APathManager::GetSplineForIndices(int32 StartIdx, int32 EndIdx) const
{
    const int32 Index = FlatIndex(StartIdx, EndIdx);

    if (!SplineMatrix.IsValidIndex(Index))
    {
        return nullptr;
    }

    USplineComponent* Spline = SplineMatrix[Index];

    return Spline;
}


int32 APathManager::GetClosestStartIndex(const FVector& WorldPos) const
{
    int32 BestIdx = INDEX_NONE;
    float BestDistSq = TNumericLimits<float>::Max();

    for (int32 i = 0; i < StartPoints.Num(); ++i)
    {
        if (AActor* A = StartPoints[i])
        {
            const float D = FVector::DistSquared(WorldPos, A->GetActorLocation());
            if (D < BestDistSq)
            {
                BestDistSq = D;
                BestIdx = i;
            }
        }
    }
    return BestIdx;
}
