#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridSnapComponent.generated.h"

class AGridGenerator;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UGridSnapComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UGridSnapComponent();

    UPROPERTY(EditAnywhere, Category = "Grid Snap")
    bool bSnapZ = false;

    UPROPERTY(EditAnywhere, Category = "Grid Snap")
    bool bOverrideGridSize = false;

    UPROPERTY(EditAnywhere, Category = "Grid Snap", meta = (EditCondition = "bOverrideGridSize", ClampMin = "1.0"))
    float GridSizeOverride = 100.f;

protected:
#if WITH_EDITOR
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostEditUndo() override;

private:
    void HandleEndObjectMovement(UObject& Object);
    void SnapOwner();
    float ResolveGridSize() const;

    FVector Snap(const FVector& In, float Size) const;

#if WITH_EDITORONLY_DATA
    mutable TWeakObjectPtr<AGridGenerator> CachedGrid;
#endif
#endif
};
