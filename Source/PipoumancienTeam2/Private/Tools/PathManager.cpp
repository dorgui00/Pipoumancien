// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/PathManager.h"
#include "Tools/SplinePathGenerator.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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
        UE_LOG(LogTemp, Warning, TEXT("PathManager: No SourceGenerator found."));
        return;
    }

    StartPoints = SourceGenerator->StartPoints;
    EndPoints = SourceGenerator->EndPoints;

    StartPoints.RemoveAll([](AActor* A) { return A == nullptr; });
    EndPoints.RemoveAll([](AActor* A) { return A == nullptr; });

    if (StartPoints.Num() == 0 || EndPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("PathManager: Need at least one StartPoint and one EndPoint."));
        return;
    }

    SplineMatrix.SetNumZeroed(StartPoints.Num() * EndPoints.Num());

    for (int32 StartIdx = 0; StartIdx < StartPoints.Num(); ++StartIdx)
    {
        AActor* Start = StartPoints[StartIdx];
        if (!Start) continue;

        for (int32 EndIdx = 0; EndIdx < EndPoints.Num(); ++EndIdx)
        {
            AActor* End = EndPoints[EndIdx];
            if (!End) continue;

            USplineComponent* Spline = NewObject<USplineComponent>(this, USplineComponent::StaticClass(), NAME_None, RF_Transactional);
            Spline->SetupAttachment(RootComponent);
            Spline->RegisterComponent();
            Spline->SetMobility(EComponentMobility::Movable);

            Spline->ClearSplinePoints(false);
            Spline->AddSplinePoint(Start->GetActorLocation(), ESplineCoordinateSpace::World, false);
            Spline->AddSplinePoint(End->GetActorLocation(), ESplineCoordinateSpace::World, true);
            Spline->SetClosedLoop(false);
            Spline->SetSplinePointType(0, ESplinePointType::Curve, false);
            Spline->SetSplinePointType(1, ESplinePointType::Curve, true);

            const int32 FI = FlatIndex(StartIdx, EndIdx);
            SplineMatrix[FI] = Spline;
        }
    }
}

void APathManager::ClearRuntimeSplines()
{
    for (USplineComponent* S : SplineMatrix)
    {
        if (S)
        {
            S->DestroyComponent();
        }
    }
    SplineMatrix.Empty();
}

AActor* APathManager::GetEndPoint(int32 EndIdx) const
{
    return EndPoints.IsValidIndex(EndIdx) ? EndPoints[EndIdx] : nullptr;
}

USplineComponent* APathManager::GetSplineForIndices(int32 StartIdx, int32 EndIdx) const
{
    if (!StartPoints.IsValidIndex(StartIdx) || !EndPoints.IsValidIndex(EndIdx))
    {
        return nullptr;
    }
    const int32 FI = FlatIndex(StartIdx, EndIdx);
    return SplineMatrix.IsValidIndex(FI) ? SplineMatrix[FI] : nullptr;
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
