#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "SplinePathGenerator.generated.h"

USTRUCT()
struct FSavedSplineData
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<FVector> Points;

    UPROPERTY()
    bool bClosedLoop = false;
};

UCLASS()
class PIPOUMANCIENTEAM2_API ASplinePathGenerator : public AActor
{
    GENERATED_BODY()

public:
    ASplinePathGenerator();

    UPROPERTY(VisibleAnywhere, Category = "Splines")
    TArray<USplineComponent*> GeneratedSplines;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    TArray<AActor*> StartPoints;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    TArray<AActor*> EndPoints;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    TSubclassOf<AActor> StartPointClass;

    UPROPERTY(EditAnywhere, Category = "Spline Setup")
    bool bShowDebug = true;

#if WITH_EDITOR
    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void RegenerateSplines();

    UFUNCTION(CallInEditor, Category = "Spline Tools")
    void RestorePreviousSplines();

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

#if WITH_EDITORONLY_DATA
    UPROPERTY(Transient)
    TArray<FSavedSplineData> BackupSplines;
#endif

protected:
    virtual void BeginPlay() override;
};
