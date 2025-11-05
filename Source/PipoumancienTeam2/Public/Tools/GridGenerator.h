#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridGenerator.generated.h"

class UStaticMesh;

UCLASS()
class PIPOUMANCIENTEAM2_API AGridGenerator : public AActor
{
    GENERATED_BODY()

public:
    AGridGenerator();

protected:
    virtual void BeginPlay() override;
    virtual void OnConstruction(const FTransform& Transform) override;

public:
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float GridSize = 100.f;

    UPROPERTY(EditAnywhere, Category = "Grid")
    float GridExtent = 5000.f;

    bool bShowGrid = true;

    UPROPERTY(EditAnywhere, Category = "Placement", meta = (MetaClass = "Actor", AllowAbstract = "false"))
    TArray<TSoftClassPtr<AActor>> PlaceableItems;

    UPROPERTY(VisibleAnywhere, Category = "Placement")
    int32 SelectedItemIndex = 0;

#if WITH_EDITOR
    FVector GetSnappedLocation(const FVector& WorldLocation) const;
    void PlaceItemAtCursor();

    UFUNCTION(CallInEditor, Category = "Placement")
    void SelectNextItem();

    UFUNCTION(CallInEditor, Category = "Placement")
    void SelectPreviousItem();

    UFUNCTION(CallInEditor, Category = "Placement")
    void PlaceSelectedItem();

    UFUNCTION(CallInEditor, Category = "Debug")
    void ToggleDebugLines();

    UFUNCTION(CallInEditor, Category = "Debug")
    void RefreshGrid();

#endif
};