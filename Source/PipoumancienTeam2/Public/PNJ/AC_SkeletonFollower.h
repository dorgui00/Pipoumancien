#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "AC_SkeletonFollower.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UAC_SkeletonFollower : public UActorComponent
{
    GENERATED_BODY()

public:
    UAC_SkeletonFollower();

    UPROPERTY(EditAnywhere)
    FName PlayersTag = "PipouPlayer";

    TArray<AActor*> PipouPlayers;
    AActor* ParentActor = nullptr;

    const float FollowRange = 600.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Players", meta = (ClampMin = "0"))
    float MoveSpeed = 10.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    USplineComponent* SplineToFollow = nullptr;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline", meta = (ClampMin = "0"))
    float SplineFollowSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    bool bOrientToSpline = true;

    UPROPERTY(EditAnywhere, Category = "PathGen|Circles", meta = (ClampMin = "0"))
    float PlayerCircleRadius = 300.f;

    UPROPERTY(EditAnywhere, Category = "PathGen|Timing", meta = (ClampMin = "0.01"))
    float SegmentDelay = 2.5f;

    UPROPERTY(EditAnywhere, Category = "PathGen|Debug")
    bool bDrawDebug = true;

    UPROPERTY(EditAnywhere, Category = "PathGen|Nav")
    float ClearanceRadius = 34.f; //character radius

    UPROPERTY(EditAnywhere, Category = "PathGen|Nav")
    float CapsuleHalfHeight = 88.f; //character half height

    UPROPERTY(EditAnywhere, Category = "PathGen|Nav")
    FVector NavQueryExtent = FVector(200.f, 200.f, 300.f);  //projection

    UPROPERTY(EditAnywhere, Category = "PathGen|Nav")
    float AdjustSearchRadius = 150.f;

    UPROPERTY(EditAnywhere, Category = "PathGen|Nav")
    float AdjustAngularStepDeg = 30.f;


    UPROPERTY(EditAnywhere, Category = "Follow|Ground")
    bool bSnapToGround = true;

    UPROPERTY(EditAnywhere, Category = "Follow|Ground", meta = (ClampMin = "0"))
    float GroundTraceUp = 200.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Ground", meta = (ClampMin = "0"))
    float GroundTraceDown = 1200.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Ground")
    float GroundOffset = 50.f;


    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    bool bYawOnly = true;


protected:
    virtual void BeginPlay() override;

    bool bPlayer1InRange = false;
    bool bPlayer2InRange = false;
    FVector Player1Location;
    FVector Player2Location;

    bool bStartFollowing = false;
    bool bFollowingSpline = false;

    float CurrentDistance = 0.f;
    float TargetDistance = 0.f;

    UFUNCTION()
    void OnParentHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnParentOverlap(AActor* OverlappedActor, AActor* OtherActor);

public:
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    void CheckPlayerRange();

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Debug")
    void ClearAllDebugLines();

private:

    USplineComponent* FindNearestSplineToOwner(bool bVillageOnly = false) const;

    void EnsureGeneratedSpline();

    void StartPathGeneration();

    void GenerateNextPathPoint();

    void StartFollowingSplineFromClosestPoint();

    void TickFollowSpline(float DeltaTime);

    FTimerHandle SegmentTimerHandle;

    bool FindWalkablePoint(const FVector& Start, const FVector& Desired, FVector& OutAdjusted) const;

    bool HasLocalClearanceAt(const FVector& Location) const;

    bool TrySnapToGround(const FVector& In, FVector& Out) const;
};
