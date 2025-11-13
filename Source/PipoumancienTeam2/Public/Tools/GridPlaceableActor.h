#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridPlaceableActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;
class AGridGenerator;

UCLASS()
class PIPOUMANCIENTEAM2_API AGridPlaceableActor : public AActor
{
    GENERATED_BODY()

public:
    AGridPlaceableActor();

    void InitializeFromMesh(UStaticMesh* InMesh);

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    UStaticMeshComponent* MeshComp = nullptr;

#if WITH_EDITOR
protected:

    virtual void PostEditMove(bool bFinished) override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

private:
    void SnapToGrid();
    float GetGridSizeFromGenerator() const;
#endif

#if WITH_EDITORONLY_DATA
    mutable TWeakObjectPtr<AGridGenerator> CachedGrid;
#endif
};