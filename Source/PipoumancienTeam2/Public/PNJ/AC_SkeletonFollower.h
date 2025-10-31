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

    const float FollowRange = 300.f;
    UPROPERTY(EditAnywhere, Category = "Follow|Players", meta = (ClampMin = "0"))
    float MoveSpeed = 10.f;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    USplineComponent* SplineToFollow = nullptr;

    UPROPERTY(EditAnywhere, Category = "Follow|Spline", meta = (ClampMin = "0"))
    float SplineFollowSpeed = 300.f;    

    UPROPERTY(EditAnywhere, Category = "Follow|Spline")
    bool bOrientToSpline = true;

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
    void FollowPlayers(float DeltaTime);

    UFUNCTION(BlueprintCallable, CallInEditor, Category = "Debug")
    void ClearAllDebugLines();

private:
    USplineComponent* FindNearestSplineToOwner() const;
    void StartFollowingSplineFromClosestPoint();
    void TickFollowSpline(float DeltaTime);
};