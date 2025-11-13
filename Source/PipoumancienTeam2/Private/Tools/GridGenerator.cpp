#include "Tools/GridGenerator.h"

#include "Engine/StaticMesh.h"
#include "Tools/GridPlaceableActor.h"
#include "Tools/GridSnapComponent.h"

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
#endif

#if WITH_EDITOR
void AGridGenerator::PlaceItemAtCursor()
{
    FLevelEditorViewportClient* ViewportClient = GCurrentLevelEditingViewportClient;
    if (!ViewportClient || !ViewportClient->Viewport) return;

    FIntPoint MousePos;
    ViewportClient->Viewport->GetMousePos(MousePos);

    FSceneViewFamilyContext ViewFamily(
        FSceneViewFamily::ConstructionValues(
            ViewportClient->Viewport,
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

    if (!PlaceableItems.IsValidIndex(SelectedItemIndex)) return;
    UClass* SelectedClass = PlaceableItems[SelectedItemIndex].LoadSynchronous();
    if (!SelectedClass) return;

    const FScopedTransaction Tx(FText::FromString(TEXT("Place Item on Grid")));
    AActor* NewActor = GetWorld()->SpawnActor<AActor>(SelectedClass, SnappedLoc, FRotator::ZeroRotator);

    if (NewActor)
    {
        NewActor->Modify();

        UGridSnapComponent* Snap = NewActor->FindComponentByClass<UGridSnapComponent>();
        if (!Snap)
        {
            Snap = NewObject<UGridSnapComponent>(
                NewActor,
                UGridSnapComponent::StaticClass(),
                TEXT("GridSnapComponent"),
                RF_Transactional
            );

            NewActor->AddInstanceComponent(Snap);
            Snap->OnComponentCreated();
            Snap->RegisterComponent();

            //override past behaviour
            Snap->bSnapZ = false;
            Snap->bOverrideGridSize = false;

            NewActor->RerunConstructionScripts();
        }

        if (Snap && !GetWorld()->IsGameWorld())
        {
            NewActor->SetActorLocation(SnappedLoc, false, nullptr, ETeleportType::TeleportPhysics);
        }
    }

}
#endif

#if WITH_EDITOR
#pragma region Buttons
void AGridGenerator::SelectNextItem()
{
    if (PlaceableItems.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex + 1) % PlaceableItems.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected actor: %s"), *GetNameSafe(PlaceableItems[SelectedItemIndex].Get()));
}
#endif

#if WITH_EDITOR
void AGridGenerator::SelectPreviousItem()
{
    if (PlaceableItems.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex - 1 + PlaceableItems.Num()) % PlaceableItems.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected actor: %s"), *GetNameSafe(PlaceableItems[SelectedItemIndex].Get()));
}
#endif

#if WITH_EDITOR
void AGridGenerator::PlaceSelectedItem()
{
    PlaceItemAtCursor();
}
#endif


#if WITH_EDITOR
void AGridGenerator::ToggleDebugLines()
{
    bShowGrid = !bShowGrid;

    if (!bShowGrid && GetWorld())
    {
        FlushPersistentDebugLines(GetWorld());
    }

    RerunConstructionScripts();

}
#endif

#if WITH_EDITOR
void AGridGenerator::RefreshGrid()
{
    if (GetWorld() && !GetWorld()->IsGameWorld())
    {
        FlushPersistentDebugLines(GetWorld());
        RerunConstructionScripts();
    }

}
#endif
#pragma endregion