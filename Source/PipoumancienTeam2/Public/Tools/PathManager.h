// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "PathManager.generated.h"

class ASplinePathGenerator;

UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API APathManager : public AActor
{
    GENERATED_BODY()
public:
    APathManager();

    UPROPERTY(EditInstanceOnly, Category = "PathManager")
    ASplinePathGenerator* SourceGenerator = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "PathManager")
    TArray<AActor*> StartPoints;

    UPROPERTY(VisibleAnywhere, Category = "PathManager")
    TArray<AActor*> EndPoints;

    UFUNCTION(BlueprintCallable, Category = "PathManager")
    AActor* GetEndPoint(int32 EndIdx) const;

    UFUNCTION(BlueprintCallable, Category = "PathManager")
    USplineComponent* GetSplineForIndices(int32 StartIdx, int32 EndIdx) const;

    UFUNCTION(BlueprintCallable, Category = "PathManager")
    int32 GetClosestStartIndex(const FVector& WorldPos) const;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Transient)
    TArray<USplineComponent*> SplineMatrix;

    int32 NumStarts() const { return StartPoints.Num(); }
    int32 NumEnds()   const { return EndPoints.Num(); }

    int32 FlatIndex(int32 StartIdx, int32 EndIdx) const
    {
        return StartIdx * NumEnds() + EndIdx;
    }

    void BuildFromGenerator();
    void ClearRuntimeSplines();
};