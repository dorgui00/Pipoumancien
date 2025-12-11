// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VillagePathManager.generated.h"

class APathManager;
class USplineComponent;

USTRUCT(BlueprintType)
struct FSkeletonVillageRoute
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Route")
    AActor* Skeleton = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Route")
    AActor* EndPoint = nullptr;
};

UCLASS()
class PIPOUMANCIENTEAM2_API AVillagePathManager : public AActor
{
    GENERATED_BODY()

public:
    AVillagePathManager();

    UPROPERTY(EditAnywhere, Category = "Village")
    APathManager* PathManager = nullptr;

    UPROPERTY(EditAnywhere, Category = "Village")
    TArray<FSkeletonVillageRoute> SkeletonRoutes;

    UFUNCTION(BlueprintCallable, Category = "Village")
    USplineComponent* GetSplineForSkeleton(AActor* Skeleton) const;

protected:
    virtual void BeginPlay() override;
};
