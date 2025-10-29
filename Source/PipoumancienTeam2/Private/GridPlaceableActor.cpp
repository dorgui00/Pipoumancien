#include "GridPlaceableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "GridGenerator.h"

AGridPlaceableActor::AGridPlaceableActor()
{
    PrimaryActorTick.bCanEverTick = false;

    MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    SetRootComponent(MeshComp);

    MeshComp->SetMobility(EComponentMobility::Static);
    MeshComp->SetCollisionProfileName(TEXT("BlockAll"));
}

void AGridPlaceableActor::InitializeFromMesh(UStaticMesh* InMesh)
{
    if (MeshComp && InMesh)
    {
        MeshComp->SetStaticMesh(InMesh);
    }
}

#if WITH_EDITOR

void AGridPlaceableActor::PostEditMove(bool bFinished)
{
    Super::PostEditMove(bFinished);
    if (bFinished)
    {
        SnapToGrid();
    }
}

void AGridPlaceableActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SnapToGrid();
}

void AGridPlaceableActor::SnapToGrid()
{
    const float GridSize = GetGridSizeFromGenerator();

    FVector L = GetActorLocation();
    L.X = FMath::GridSnap(L.X, GridSize);
    L.Y = FMath::GridSnap(L.Y, GridSize);
    SetActorLocation(L, false, nullptr, ETeleportType::TeleportPhysics);
}

float AGridPlaceableActor::GetGridSizeFromGenerator() const
{
    if (CachedGrid.IsValid())
    {
        return CachedGrid.Get()->GridSize;
    }

    float BestDistSq = TNumericLimits<float>::Max();
    AGridGenerator* Best = nullptr;

    if (UWorld* World = GetWorld())
    {
        for (TActorIterator<AGridGenerator> It(World); It; ++It)
        {
            const float DistSq = FVector::DistSquared(It->GetActorLocation(), GetActorLocation());
            if (DistSq < BestDistSq)
            {
                BestDistSq = DistSq;
                Best = *It;
            }
        }
    }

    if (Best)
    {
        CachedGrid = Best;
        return Best->GridSize;
    }

    return 100.f;
}
#endif