#include "GridPlaceableActor.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

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
