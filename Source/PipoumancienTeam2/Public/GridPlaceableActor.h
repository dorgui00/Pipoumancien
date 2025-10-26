#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridPlaceableActor.generated.h"

class UStaticMesh;
class UStaticMeshComponent;

UCLASS()
class PIPOUMANCIENTEAM2_API AGridPlaceableActor : public AActor
{
    GENERATED_BODY()

public:
    AGridPlaceableActor();

    UFUNCTION()
    void InitializeFromMesh(UStaticMesh* InMesh);

    UFUNCTION(BlueprintCallable, Category = "Grid")
    UStaticMeshComponent* GetMeshComponent() const { return MeshComp; }

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Grid")
    UStaticMeshComponent* MeshComp = nullptr;
};
