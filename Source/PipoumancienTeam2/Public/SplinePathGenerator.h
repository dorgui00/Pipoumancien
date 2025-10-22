#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplinePathGenerator.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API ASplinePathGenerator : public AActor
{
    GENERATED_BODY()

public:
    ASplinePathGenerator();

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    AActor* StartPoint;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    TArray<AActor*> EndPoints;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    bool bShowDebug = true;


#if WITH_EDITOR

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void RegenerateSplines();

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void ClearSplines();

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void AddEndpoint();

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void AddStartPoint();

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void UpdateDebugVisuals();

#endif


private:
    UPROPERTY(Transient)
    TArray<USplineComponent*> GeneratedSplines;
};
