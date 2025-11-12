
#include "PNJ/AC_SkeletonFollower.h"

#include "Character/PipouCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SplineComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "CollisionShape.h" 

UAC_SkeletonFollower::UAC_SkeletonFollower()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UAC_SkeletonFollower::BeginPlay()
{
    Super::BeginPlay();

    UGameplayStatics::GetAllActorsWithTag(GetWorld(), PlayersTag, PipouPlayers);
    ParentActor = GetOwner();

    if (ParentActor)
    {
        ParentActor->OnActorHit.AddDynamic(this, &UAC_SkeletonFollower::OnParentHit);
        ParentActor->OnActorBeginOverlap.AddDynamic(this, &UAC_SkeletonFollower::OnParentOverlap);
    }
}

void UAC_SkeletonFollower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (bFollowingSpline && SplineToFollow)
    {
        TickFollowSpline(DeltaTime);
    }

    CheckPlayerRange();
}


void UAC_SkeletonFollower::CheckPlayerRange()
{
    if (bStartFollowing) return;

    if (!ParentActor) return;

    const FVector ParentLocation = ParentActor->GetActorLocation();

    bPlayer1InRange = false;
    bPlayer2InRange = false;

    for (int32 i = 0; i < PipouPlayers.Num(); i++)
    {
        if (AActor* Player = PipouPlayers[i])
        {
            const float Distance = FVector::Dist(Player->GetActorLocation(), ParentLocation);
            if (Distance <= FollowRange)
            {
                if (i == 0) bPlayer1InRange = true;
                if (i == 1) bPlayer2InRange = true;
            }
        }
    }

    if (bPlayer1InRange && bPlayer2InRange && PipouPlayers.Num() >= 2)
    {
        bStartFollowing = true;
        StartPathGeneration();
    }
}


void UAC_SkeletonFollower::EnsureGeneratedSpline()
{
    if (SplineToFollow) return;

    if (!ParentActor) return;

    SplineToFollow = NewObject<USplineComponent>(ParentActor, TEXT("GeneratedFollowSpline"));
    SplineToFollow->SetupAttachment(ParentActor->GetRootComponent());
    SplineToFollow->RegisterComponent();

    const FVector StartLoc = ParentActor->GetActorLocation();
    SplineToFollow->ClearSplinePoints(false);
    SplineToFollow->AddSplinePoint(StartLoc, ESplineCoordinateSpace::World, false);
    SplineToFollow->SetSplinePointType(0, ESplinePointType::Linear, false);
    SplineToFollow->UpdateSpline();

    StartFollowingSplineFromClosestPoint();
}

void UAC_SkeletonFollower::StartPathGeneration()
{
    EnsureGeneratedSpline();

    if (UWorld* World = GetWorld())
    {
        World->GetTimerManager().ClearTimer(SegmentTimerHandle);

        if (SegmentDelay > 0.f)
        {
            World->GetTimerManager().SetTimer(
                SegmentTimerHandle,
                this,
                &UAC_SkeletonFollower::GenerateNextPathPoint,
                SegmentDelay,
                true
            );
        }
        else
        {
            GenerateNextPathPoint();
        }
    }
}

static FVector ClosestPointOnPlayerCircle(const FVector& PlayerPos, const FVector& FromPos, float Radius)
{
    const FVector Dir = (FromPos - PlayerPos).GetSafeNormal();
    return PlayerPos + Dir * Radius;
}

void UAC_SkeletonFollower::GenerateNextPathPoint()
{
    if (!ParentActor || PipouPlayers.Num() < 2) return;

    Player1Location = PipouPlayers[0] ? PipouPlayers[0]->GetActorLocation() : FVector::ZeroVector;
    Player2Location = PipouPlayers[1] ? PipouPlayers[1]->GetActorLocation() : FVector::ZeroVector;

    const FVector SkelPos = ParentActor->GetActorLocation();

    if (bDrawDebug)
    {
        DrawDebugCircle(GetWorld(), Player1Location, PlayerCircleRadius, 32, FColor::Red, false, SegmentDelay * 1.1f, 0, 2.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
        DrawDebugCircle(GetWorld(), Player2Location, PlayerCircleRadius, 32, FColor::Blue, false, SegmentDelay * 1.1f, 0, 2.f, FVector(1, 0, 0), FVector(0, 1, 0), false);
    }

    const FVector P1OnCircle = ClosestPointOnPlayerCircle(Player1Location, SkelPos, PlayerCircleRadius);
    const FVector P2OnCircle = ClosestPointOnPlayerCircle(Player2Location, SkelPos, PlayerCircleRadius);

    if (bDrawDebug)
    {
        DrawDebugPoint(GetWorld(), P1OnCircle, 12.f, FColor::Red, false, SegmentDelay * 1.1f);
        DrawDebugPoint(GetWorld(), P2OnCircle, 12.f, FColor::Blue, false, SegmentDelay * 1.1f);
        DrawDebugLine(GetWorld(), P1OnCircle, P2OnCircle, FColor::Purple, false, SegmentDelay * 1.1f, 0, 1.5f);
    }

    const FVector Mid = (P1OnCircle + P2OnCircle) * 0.5f;

    if (!SplineToFollow)
    {
        EnsureGeneratedSpline();
    }

    if (SplineToFollow)
    {
        const int32 LastIdx = SplineToFollow->GetNumberOfSplinePoints() - 1;
        const FVector StartLoc = SplineToFollow->GetLocationAtSplinePoint(LastIdx, ESplineCoordinateSpace::World);

        FVector Adjusted;
        if (!FindWalkablePoint(StartLoc, Mid, Adjusted))
        {
            Adjusted = Mid;
        }

        SplineToFollow->AddSplinePoint(Adjusted, ESplineCoordinateSpace::World, false);
        const int32 NewIdx = SplineToFollow->GetNumberOfSplinePoints() - 1;
        SplineToFollow->SetSplinePointType(NewIdx, ESplinePointType::Linear, false);
        SplineToFollow->UpdateSpline();

        if (bDrawDebug)
        {
            DrawDebugPoint(GetWorld(), Adjusted, 16.f, FColor::Green, false, SegmentDelay * 1.1f);
            const FVector Prev = SplineToFollow->GetLocationAtSplinePoint(NewIdx - 1, ESplineCoordinateSpace::World);
            DrawDebugLine(GetWorld(), Prev, Adjusted, FColor::Green, false, SegmentDelay * 1.1f, 0, 2.f);
        }
    }


}

USplineComponent* UAC_SkeletonFollower::FindNearestSplineToOwner(bool bVillageOnly) const
{
    UWorld* World = GetWorld();
    AActor* Owner = GetOwner();
    if (!World || !Owner) return nullptr;

    USplineComponent* BestSpline = nullptr;
    float BestDistSq = TNumericLimits<float>::Max();
    const FVector OwnerLoc = Owner->GetActorLocation();

    for (TActorIterator<AActor> It(World); It; ++It)
    {
        AActor* A = *It;
        if (!A) continue;

        if (A == Owner) continue;

        TInlineComponentArray<USplineComponent*> Splines(A);
        for (USplineComponent* Spline : Splines)
        {
            if (!Spline) continue;

            if (bVillageOnly && !Spline->ComponentTags.Contains(FName("VillageSpline")))
            {
                continue;
            }

            const float Key = Spline->FindInputKeyClosestToWorldLocation(OwnerLoc);
            const FVector Closest = Spline->GetLocationAtSplineInputKey(Key, ESplineCoordinateSpace::World);
            const float DistSq = FVector::DistSquared(OwnerLoc, Closest);

            if (DistSq < BestDistSq)
            {
                BestDistSq = DistSq;
                BestSpline = Spline;
            }
        }
    }
    return BestSpline;
}

void UAC_SkeletonFollower::StartFollowingSplineFromClosestPoint()
{
    if (!ParentActor || !SplineToFollow) return;

    const FVector OwnerLoc = ParentActor->GetActorLocation();
    const float ClosestKey = SplineToFollow->FindInputKeyClosestToWorldLocation(OwnerLoc);
    CurrentDistance = SplineToFollow->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);

    TargetDistance = SplineToFollow->GetSplineLength();
    bFollowingSpline = true;
}

void UAC_SkeletonFollower::TickFollowSpline(float DeltaTime)
{
    if (!ParentActor || !SplineToFollow) return;

    TargetDistance = SplineToFollow->GetSplineLength();

    CurrentDistance = FMath::Min(CurrentDistance + SplineFollowSpeed * DeltaTime, TargetDistance);

    const FVector NewLoc = SplineToFollow->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

    if (bOrientToSpline)
    {
        const FRotator NewRot = SplineToFollow->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
        ParentActor->SetActorLocationAndRotation(NewLoc, NewRot);
    }
    else
    {
        ParentActor->SetActorLocation(NewLoc);
    }
}


void UAC_SkeletonFollower::OnParentHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor && OtherActor->ActorHasTag("VillageBorder"))
    {
        bStartFollowing = false;
    }
}

void UAC_SkeletonFollower::OnParentOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (OtherActor && OtherActor->ActorHasTag("VillageBorder"))
    {
        bStartFollowing = false;

        if (UWorld* World = GetWorld())
        {
            World->GetTimerManager().ClearTimer(SegmentTimerHandle);
        }

        if (SplineToFollow)
        {
            if (SplineToFollow->GetOwner() == GetOwner())
            {
                SplineToFollow->DestroyComponent();
            }
            SplineToFollow = nullptr;
            bFollowingSpline = false;
        }

        USplineComponent* NearestVillage = FindNearestSplineToOwner(true);
        if (NearestVillage)
        {
            SplineToFollow = NearestVillage;
            StartFollowingSplineFromClosestPoint();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("SkeletonFollower: No village spline found nearby."));
        }
    }
}

//nav

bool UAC_SkeletonFollower::HasLocalClearanceAt(const FVector& Location) const
{
    UWorld* World = GetWorld();
    if (!World || !ParentActor) return false;

    FCollisionQueryParams Params(SCENE_QUERY_STAT(SplinePointClearance), false, ParentActor);
    FCollisionShape Capsule = FCollisionShape::MakeCapsule(ClearanceRadius, CapsuleHalfHeight);

    FVector Start = Location + FVector(0, 0, 5.f);
    FVector End = Location - FVector(0, 0, 5.f);

    FHitResult Hit;
    const bool bHit = World->SweepSingleByChannel(
        Hit,
        Start,
        End,
        FQuat::Identity,

        //PLACEHOLDER

        ECC_Pawn,

        //SET TO CORRECT COLLISION CHANNEL FOR SKELETON NPC's
        //PLACEHOLDER


        Capsule,
        Params
    );

    return !bHit;
}

bool UAC_SkeletonFollower::FindWalkablePoint(const FVector& Start, const FVector& Desired, FVector& OutAdjusted) const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World);
    if (!NavSys) return false;

    FNavLocation Projected;
    if (!NavSys->ProjectPointToNavigation(Desired, Projected, NavQueryExtent))
    {
        if (!NavSys->ProjectPointToNavigation(Start, Projected, NavQueryExtent))
            return false;
    }

    UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(World, Start, Projected.Location);
    if (Path && Path->IsValid())
    {
        if (!Path->IsPartial())
        {
            FVector Candidate = Projected.Location;
            if (HasLocalClearanceAt(Candidate))
            {
                OutAdjusted = Candidate;
                return true;
            }
        }

        if (Path->PathPoints.Num() > 0)
        {
            for (int32 i = Path->PathPoints.Num() - 1; i >= 0; --i)
            {
                const FVector Candidate = Path->PathPoints[i];
                if (HasLocalClearanceAt(Candidate))
                {
                    OutAdjusted = Candidate;
                    return true;
                }
            }
        }
    }

    const float StepRad = FMath::DegreesToRadians(AdjustAngularStepDeg);
    for (float Angle = 0.f; Angle < 2 * PI; Angle += StepRad)
    {
        const FVector Offset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0.f) * AdjustSearchRadius;
        FNavLocation Around;
        if (NavSys->ProjectPointToNavigation(Desired + Offset, Around, NavQueryExtent))
        {
            UNavigationPath* AltPath = NavSys->FindPathToLocationSynchronously(World, Start, Around.Location);
            if (AltPath && AltPath->IsValid() && !AltPath->IsPartial())
            {
                if (HasLocalClearanceAt(Around.Location))
                {
                    OutAdjusted = Around.Location;
                    return true;
                }
            }
        }
    }

    return false;
}


//debug

void UAC_SkeletonFollower::ClearAllDebugLines()
{
    if (UWorld* World = GetWorld())
    {
        FlushPersistentDebugLines(World);
        FlushDebugStrings(World);
    }
}
