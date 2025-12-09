#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "AC_SkeletonFollower.generated.h"

class APipouCharacter;

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

    const float FollowRange = 500.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Players", meta = (ClampMin = "0"))
    float MoveSpeed = 10.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    USplineComponent* SplineToFollow = nullptr;

    // speed
    UPROPERTY(EditAnywhere, Category = "Follow|Spline", meta = (ClampMin = "0"))
    float SplineFollowSpeed = 300.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    bool bOrientToSpline = true;

    UFUNCTION(BlueprintCallable, Category = "Follow")
    bool IsStartFollowing() const { return bStartFollowing; }

    // distance par rapport au joueur
    UPROPERTY(EditAnywhere, Category = "PathGen|Circles", meta = (ClampMin = "0"))
    float PlayerCircleRadius = 300.f;

    // delay
    UPROPERTY(EditAnywhere, Category = "PathGen|Timing", meta = (ClampMin = "0.01"))
    float SegmentDelay = 0.1f;

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
    float GroundOffset = 0.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Ground", meta = (ClampMin = "10"))
    float BacktrackStepSize = 100.f;


    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    bool bYawOnly = true;



    // DELEGATES
    
    // On Enter Village
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterVillage);

    // Waiting for dialogue
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaitingForDialogue);

    UPROPERTY()
    FOnEnterVillage OnEnterVillage;
    
    // DON'T FORGET TO BROADCAST
    // On Reach Home
    DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReachHome);
    
    UPROPERTY()
    FOnReachHome OnReachHome;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline", meta = (ClampMin = "0"))
    float SplineEntryLerpTime = 0.5f;

    //NIAGARA

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
        FOnPipouFollowEffectChanged,
        APipouCharacter*, Pipou,
        bool, bActive);

    UPROPERTY(BlueprintAssignable, Category = "Follow|Events")
    FOnPipouFollowEffectChanged OnPipouFollowEffectChanged;

    UPROPERTY(BlueprintAssignable, Category = "Events")
    FOnWaitingForDialogue OnWaitingForDialogue;

    UFUNCTION(BlueprintCallable, Category = "Follow|Spline")
    void ResumeFollowingSpline();

protected:
    virtual void BeginPlay() override;

    bool bPlayer1InRange = false;
    bool bPlayer2InRange = false;
    FVector Player1Location = FVector::ZeroVector;
    FVector Player2Location = FVector::ZeroVector;

    bool bStartFollowing = false;
    bool bFollowingSpline = false;

    bool bOnVillageSpline = false;
    bool bHasReachedHome = false;

    bool bCanFollowPlayers = true;

    float CurrentDistance = 0.f;
    float TargetDistance = 0.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Players", meta = (ClampMin = "0"))
    float PlayerMovingSpeedThreshold = .5f;

    UPROPERTY(EditAnywhere, Category = "Follow|Players", meta = (ClampMin = "0"))
    float PlayerMovingDistanceThreshold = .5f;

    UPROPERTY(Transient)
    TArray<FVector> PreviousPlayerLocations;

    UPROPERTY(Transient)
    bool bHasPreviousPlayerLocations = false;

    UPROPERTY(Transient)
    bool bAnyPlayerMoving = false;

    UFUNCTION(BlueprintCallable, Category = "Follow|Players")
    bool IsAnyPlayerMoving() const { return bAnyPlayerMoving; }

    void UpdatePlayerMovement(float DeltaTime);


    UFUNCTION()
    void OnParentHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

    UFUNCTION()
    void OnParentOverlap(AActor* OverlappedActor, AActor* OtherActor);

    UFUNCTION()
    void HandleReachHome();

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

    void StartFollowingSplineFromClosestPoint(bool bLerpToStart = false);

    void TickLerpToSpline(float DeltaTime);

    void TickFollowSpline(float DeltaTime);

    FTimerHandle SegmentTimerHandle;

    bool FindWalkablePoint(const FVector& Start, const FVector& Desired, FVector& OutAdjusted) const;

    bool HasLocalClearanceAt(const FVector& Location) const;

    bool TrySnapToGround(const FVector& In, FVector& Out) const;

    bool bLerpingToSpline = false;
    FVector LerpStartLocation = FVector::ZeroVector;
    FVector LerpTargetLocation = FVector::ZeroVector;
    float LerpElapsedTime = 0.f;

    bool BacktrackToGround(const FVector& Start, const FVector& End, FVector& Out) const;
};
