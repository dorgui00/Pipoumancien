#include "GridGenerator.h"

#include "Engine/StaticMesh.h"
#include "GridPlaceableActor.h"

#if WITH_EDITOR
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "LevelEditorViewport.h"
#include "EditorViewportClient.h"
#include "SceneView.h"
#endif



void AGridGenerator::BeginPlay()
{
    Super::BeginPlay();

    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
}

void AGridGenerator::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

AGridGenerator::AGridGenerator()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AGridGenerator::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (GetWorld() && GetWorld()->IsGameWorld())
    {
        SetActorHiddenInGame(true);
        SetActorEnableCollision(false);
        return;
    }

#if WITH_EDITOR
    if (bShowGrid)
    {
        const float HalfExtent = GridExtent;
        const int32 NumLines = FMath::CeilToInt((HalfExtent * 2) / GridSize);

        FlushPersistentDebugLines(GetWorld());

        FVector Origin = GetActorLocation();

        FHitResult Hit;
        FVector Start = Origin + FVector(0, 0, 1000);
        FVector End = Origin - FVector(0, 0, 10000);

        if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_WorldStatic))
        {
            Origin.Z = Hit.ImpactPoint.Z + 5.f;
        }
        for (int32 i = -NumLines / 2; i <= NumLines / 2; ++i)
        {
            const float Offset = i * GridSize;
            FVector StartX = Origin + FVector(Offset, -HalfExtent, 0);
            FVector EndX = Origin + FVector(Offset, HalfExtent, 0);
            FVector StartY = Origin + FVector(-HalfExtent, Offset, 0);
            FVector EndY = Origin + FVector(HalfExtent, Offset, 0);

            DrawDebugLine(GetWorld(), StartX, EndX, FColor::Green, true, 0.f, 0, 5.f);
            DrawDebugLine(GetWorld(), StartY, EndY, FColor::Green, true, 0.f, 0, 5.f);
        }
    }

#endif
}

#if WITH_EDITOR

FVector AGridGenerator::GetSnappedLocation(const FVector& WorldLocation) const
{
    FVector Snapped = WorldLocation;
    Snapped.X = FMath::GridSnap(Snapped.X, GridSize);
    Snapped.Y = FMath::GridSnap(Snapped.Y, GridSize);
    Snapped.Z = FMath::GridSnap(Snapped.Z, GridSize);
    return Snapped;
}

#if WITH_EDITOR
void AGridGenerator::PlaceItemAtCursor()
{
    FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
    if (!ViewportClient || !ViewportClient->Viewport) return;

    FIntPoint MousePos;
    ViewportClient->Viewport->GetMousePos(MousePos);

    FSceneViewFamilyContext ViewFamily(
        FSceneViewFamily::ConstructionValues(ViewportClient->Viewport,
            GetWorld()->Scene,
            ViewportClient->EngineShowFlags));
    FSceneView* SceneView = ViewportClient->CalcSceneView(&ViewFamily);
    if (!SceneView) return;

    FViewportCursorLocation Cursor(SceneView, ViewportClient, MousePos.X, MousePos.Y);
    const FVector RayStart = Cursor.GetOrigin();
    const FVector RayEnd = RayStart + Cursor.GetDirection() * 100000.f;

    FHitResult Hit;
    GetWorld()->LineTraceSingleByChannel(Hit, RayStart, RayEnd, ECC_Visibility);
    if (!Hit.bBlockingHit) return;

    const FVector SnappedLoc = GetSnappedLocation(Hit.Location);

    if (!PlaceableMeshes.IsValidIndex(SelectedItemIndex)) return;
    UStaticMesh* MeshToPlace = PlaceableMeshes[SelectedItemIndex].LoadSynchronous();
    if (!MeshToPlace) return;

    const FScopedTransaction Tx(FText::FromString(TEXT("Place Item on Grid")));
    AGridPlaceableActor* NewActor =
        GetWorld()->SpawnActor<AGridPlaceableActor>(AGridPlaceableActor::StaticClass(),
            SnappedLoc, FRotator::ZeroRotator);

    if (NewActor)
    {
        NewActor->Modify();
        NewActor->InitializeFromMesh(MeshToPlace);
    }
}
#endif


void AGridGenerator::SelectNextItem()
{
    if (PlaceableMeshes.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex + 1) % PlaceableMeshes.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected mesh: %s"), *GetNameSafe(PlaceableMeshes[SelectedItemIndex].Get()));
}

void AGridGenerator::SelectPreviousItem()
{
    if (PlaceableMeshes.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex - 1 + PlaceableMeshes.Num()) % PlaceableMeshes.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected mesh: %s"), *GetNameSafe(PlaceableMeshes[SelectedItemIndex].Get()));
}

void AGridGenerator::PlaceSelectedItem()
{
    PlaceItemAtCursor();
}
#endif