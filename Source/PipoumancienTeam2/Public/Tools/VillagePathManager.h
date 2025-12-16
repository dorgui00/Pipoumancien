// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CineCameraActor.h"
#include "GameFramework/Actor.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "LevelSequenceActor.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Village Route")
    AActor* HouseLight = nullptr;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    bool bReachedHome = false;
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

    UFUNCTION(BlueprintCallable, Category = "Village")
    void OnSkeletonReachedEnd(AActor* Skeleton);

    //cutscene
    UPROPERTY(EditAnywhere, Category = "Cutscene")
    ACineCameraActor* CutsceneCamera = nullptr;

    UPROPERTY(VisibleAnywhere, Category = "Cutscene")
    bool bCutsceneStarted = false;

    void TryStartCutscene();

    UPROPERTY(EditAnywhere, Category = "Cutscene")
    float CutsceneBlendTime = 1.0f;

    UPROPERTY(EditAnywhere, Category = "Cutscene")
    bool bUseCinematicMode = true;

    UPROPERTY(VisibleAnywhere, Category = "Cutscene")
    AActor* PreviousViewTarget = nullptr;

    UPROPERTY(EditAnywhere, Category = "Cutscene")
    ALevelSequenceActor* CutsceneSequenceActor = nullptr;

    UFUNCTION(BlueprintNativeEvent, Category = "Cutscene")
    void StartVillageCutscene();

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputMappingContext* CutsceneIMC = nullptr;

    UPROPERTY(EditAnywhere, Category = "Input")
    UInputAction* StartCutsceneAction = nullptr;

    UFUNCTION()
    void HandleStartCutsceneInput();

protected:
    virtual void BeginPlay() override;
};
