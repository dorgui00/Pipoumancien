#include "GridGenerator.h"

#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "ScopedTransaction.h"

#if WITH_EDITOR
#include "LevelEditor.h"
#include "SLevelViewport.h"
#include "EditorViewportClient.h"
#include "Editor.h"
#include "UnrealEdGlobals.h"
#include "Editor/UnrealEdEngine.h"
#include "ViewportWorldInteraction.h"
#include "LevelEditorViewport.h"
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

    //only in edtior
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

void AGridGenerator::PlaceItemAtCursor()
{
    if (PlaceableItems.Num() == 0) return;
    if (!PlaceableItems.IsValidIndex(SelectedItemIndex)) return;

    TSubclassOf<AActor> SelectedItem = PlaceableItems[SelectedItemIndex];
    if (!SelectedItem) return;

    FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
    TSharedPtr<ILevelEditor> LevelEditor = LevelEditorModule.GetFirstLevelEditor();
    if (!LevelEditor.IsValid()) return;

    TSharedPtr<SLevelViewport> ActiveLevelViewport = LevelEditor->GetActiveViewportInterface();
    if (!ActiveLevelViewport.IsValid()) return;

    FEditorViewportClient& ViewportClient = ActiveLevelViewport->GetLevelViewportClient();

    FIntPoint MousePos;
    if (!ViewportClient.Viewport) return;
    ViewportClient.Viewport->GetMousePos(MousePos);

    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
        ViewportClient.Viewport, GetWorld()->Scene, ViewportClient.EngineShowFlags));

    FSceneView* SceneView = ViewportClient.CalcSceneView(&ViewFamily);
    if (!SceneView) return;

    FViewportCursorLocation Cursor(SceneView, &ViewportClient, MousePos.X, MousePos.Y);
    FVector Start = Cursor.GetOrigin();
    FVector End = Start + Cursor.GetDirection() * 100000.f;

    FHitResult Hit;
    GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility);

    if (Hit.bBlockingHit)
    {
        FVector SnappedLoc = GetSnappedLocation(Hit.Location);
        const FScopedTransaction Transaction(FText::FromString(TEXT("Place Item on Grid")));
        AActor* NewActor = GetWorld()->SpawnActor<AActor>(SelectedItem, SnappedLoc, FRotator::ZeroRotator);

        if (NewActor)
        {
            NewActor->Modify(); //undo
        }
    }
}


void AGridGenerator::SelectNextItem()
{
    if (PlaceableItems.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex + 1) % PlaceableItems.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected item: %s"), *GetNameSafe(PlaceableItems[SelectedItemIndex]));
}

void AGridGenerator::SelectPreviousItem()
{
    if (PlaceableItems.Num() == 0) return;
    SelectedItemIndex = (SelectedItemIndex - 1 + PlaceableItems.Num()) % PlaceableItems.Num();
    UE_LOG(LogTemp, Log, TEXT("Selected item: %s"), *GetNameSafe(PlaceableItems[SelectedItemIndex]));
}

void AGridGenerator::PlaceSelectedItem()
{
    PlaceItemAtCursor();
}
#endif

