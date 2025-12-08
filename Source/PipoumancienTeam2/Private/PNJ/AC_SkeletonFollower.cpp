
#include "PNJ/AC_SkeletonFollower.h"
#include "PNJ/SkeletonController.h"

#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SplineComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CollisionShape.h" 
#include "Components/LightComponent.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateWalk.h"
#include "Tools/VillagePathManager.h"
#include "MyAnimNotify_PlayCleanseOnce.h"


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

    OnReachHome.AddDynamic(this, &UAC_SkeletonFollower::HandleReachHome);
}

void UAC_SkeletonFollower::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (SplineToFollow)
    {
        if (bLerpingToSpline)
        {
            TickLerpToSpline(DeltaTime);
        }
        else if (bFollowingSpline)
        {
            TickFollowSpline(DeltaTime);
        }
    }

    CheckPlayerRange();
    UpdatePlayerMovement(DeltaTime);
}


void UAC_SkeletonFollower::CheckPlayerRange()
{
    if (!bCanFollowPlayers)
        return;

    if (bStartFollowing)
        return;

    if (!ParentActor)
        return;

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

    SplineToFollow = NewObject<USplineComponent>(this, TEXT("GeneratedFollowSpline"));
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
    UE_LOG(LogTemp, Warning, TEXT("Starting StartPathGeneration();"));

    bOnVillageSpline = false;
    bHasReachedHome = false;

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
    if (!ParentActor || PipouPlayers.Num() < 2)
        return;

    if (!IsAnyPlayerMoving())
        return;

    Player1Location = PipouPlayers[0] ? PipouPlayers[0]->GetActorLocation() : FVector::ZeroVector;
    Player2Location = PipouPlayers[1] ? PipouPlayers[1]->GetActorLocation() : FVector::ZeroVector;

    const FVector SkelPos = ParentActor->GetActorLocation();

    const FVector P1OnCircle = ClosestPointOnPlayerCircle(Player1Location, SkelPos, PlayerCircleRadius);
    const FVector P2OnCircle = ClosestPointOnPlayerCircle(Player2Location, SkelPos, PlayerCircleRadius);

    const FVector Mid = (P1OnCircle + P2OnCircle) * 0.5f;

    FVector Adjusted;
    const bool bFoundNavPoint = FindWalkablePoint(SkelPos, Mid, Adjusted);

    if (!bFoundNavPoint)
    {
        Adjusted = Mid;

        if (bSnapToGround)
        {
            FVector Grounded = Adjusted;
            if (TrySnapToGround(Adjusted, Grounded))
            {
                Adjusted = Grounded;
            }
        }
    }

    if (SplineToFollow)
    {
        const int32 LastIdx = SplineToFollow->GetNumberOfSplinePoints() - 1;
        SplineToFollow->AddSplinePoint(Adjusted, ESplineCoordinateSpace::World, false);

        SplineToFollow->SetSplinePointType(LastIdx + 1, ESplinePointType::Curve, true);
        SplineToFollow->UpdateSpline();
    }

    if (!bFollowingSpline)
    {
        bFollowingSpline = true;
        StartFollowingSplineFromClosestPoint();
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

void UAC_SkeletonFollower::StartFollowingSplineFromClosestPoint(bool bLerpToStart)
{
    if (!ParentActor || !SplineToFollow) return;

    const FVector OwnerLoc = ParentActor->GetActorLocation();
    const float ClosestKey = SplineToFollow->FindInputKeyClosestToWorldLocation(OwnerLoc);

    CurrentDistance = SplineToFollow->GetDistanceAlongSplineAtSplineInputKey(ClosestKey);
    TargetDistance = SplineToFollow->GetSplineLength();

    if (bLerpToStart)
    {
        bFollowingSpline = false;
        bLerpingToSpline = true;
        LerpElapsedTime = 0.f;
        LerpStartLocation = OwnerLoc;
        LerpTargetLocation = SplineToFollow->GetLocationAtDistanceAlongSpline(
            CurrentDistance,
            ESplineCoordinateSpace::World
        );
    }
    else
    {
        bLerpingToSpline = false;
        bFollowingSpline = true;
    }
}

void UAC_SkeletonFollower::TickFollowSpline(float DeltaTime)
{
    if (!bFollowingSpline || !ParentActor || !SplineToFollow)
    {
        return;
    }

    const float SplineLength = SplineToFollow->GetSplineLength();

    CurrentDistance = FMath::Clamp(CurrentDistance + (SplineFollowSpeed * DeltaTime), 0.0f, SplineLength);

    const FVector NewLocation = SplineToFollow->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
    const FRotator SplineRotation = SplineToFollow->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);

    if (bOrientToSpline)
    {
        FRotator NewRot;

        if (bYawOnly)
        {
            FVector Dir = SplineRotation.Vector();
            Dir.Z = 0.f;

            if (!Dir.IsNearlyZero())
            {
                NewRot = Dir.Rotation();
                NewRot.Pitch = 0.f;
                NewRot.Roll = 0.f;
            }
            else
            {
                NewRot = ParentActor->GetActorRotation();
                NewRot.Pitch = 0.f;
                NewRot.Roll = 0.f;
            }
        }
        else
        {
            NewRot = SplineRotation;
        }

        ParentActor->SetActorLocationAndRotation(NewLocation, NewRot);
    }
    else
    {
        ParentActor->SetActorLocation(NewLocation);
    }

    if (CurrentDistance >= SplineLength - KINDA_SMALL_NUMBER)
    {
        bFollowingSpline = false;
        UE_LOG(LogTemp, Display, TEXT("[SkeletonFollower] %s reached the end of spline."), *ParentActor->GetName());
    }
}

void UAC_SkeletonFollower::TickLerpToSpline(float DeltaTime)
{
    if (!bLerpingToSpline || !ParentActor || !SplineToFollow)
    {
        return;
    }

    const FVector CurrentLoc = ParentActor->GetActorLocation();
    FVector ToTarget = LerpTargetLocation - CurrentLoc;
    const float DistToTarget = ToTarget.Size();

    auto HandleLerpFinished = [this]()
        {
            bLerpingToSpline = false;
            bFollowingSpline = false;

            if (ParentActor)
            {
                ParentActor->SetActorLocation(LerpTargetLocation);

                if (ASkeletonController* Skel = Cast<ASkeletonController>(ParentActor))
                {
                    Skel->StartCleanseWindow();
                }
            }

            OnWaitingForDialogue.Broadcast();

            ParentActor->SetActorRotation(FRotator(0.f, 180.f, 0.f));
        };

    if (DistToTarget <= KINDA_SMALL_NUMBER)
    {
        HandleLerpFinished();
        return;
    }

    ToTarget.Normalize();

    const float MaxStep = SplineFollowSpeed * DeltaTime;
    const bool bWillReachThisFrame = MaxStep >= DistToTarget;

    const FVector NewLoc = bWillReachThisFrame
        ? LerpTargetLocation
        : CurrentLoc + ToTarget * MaxStep;

    if (bOrientToSpline)
    {
        FRotator NewRot;
        FVector DirForRot = ToTarget;

        if (bYawOnly)
        {
            DirForRot.Z = 0.f;

            if (!DirForRot.IsNearlyZero())
            {
                NewRot = DirForRot.Rotation();
                NewRot.Pitch = 0.f;
                NewRot.Roll = 0.f;
            }
            else
            {
                NewRot = ParentActor->GetActorRotation();
                NewRot.Pitch = 0.f;
                NewRot.Roll = 0.f;
            }
        }
        else
        {
            NewRot = DirForRot.Rotation();
        }

        ParentActor->SetActorLocationAndRotation(NewLoc, NewRot);
    }
    else
    {
        ParentActor->SetActorLocation(NewLoc);
    }

    if (bWillReachThisFrame)
    {
        HandleLerpFinished();
    }
}

#pragma region Player Checks

void UAC_SkeletonFollower::UpdatePlayerMovement(float DeltaTime)
{
    bAnyPlayerMoving = false;

    if (PipouPlayers.Num() == 0 || DeltaTime <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    if (!bHasPreviousPlayerLocations || PreviousPlayerLocations.Num() != PipouPlayers.Num())
    {
        PreviousPlayerLocations.SetNum(PipouPlayers.Num());
        for (int32 i = 0; i < PipouPlayers.Num(); ++i)
        {
            if (PipouPlayers[i])
            {
                PreviousPlayerLocations[i] = PipouPlayers[i]->GetActorLocation();
            }
        }

        bHasPreviousPlayerLocations = true;
        return;
    }

    for (int32 i = 0; i < PipouPlayers.Num(); ++i)
    {
        AActor* Player = PipouPlayers[i];
        if (!Player)
            continue;

        const FVector CurrentLoc = Player->GetActorLocation();
        const FVector PrevLoc = PreviousPlayerLocations[i];

        const float DistanceMoved = FVector::Dist(CurrentLoc, PrevLoc);
        PreviousPlayerLocations[i] = CurrentLoc;

        const bool bIsMovingNow = (DistanceMoved >= PlayerMovingDistanceThreshold);
        const bool bShouldEffectBeOn = bStartFollowing && bIsMovingNow;

        if (APipouCharacter* Pipou = Cast<APipouCharacter>(Player))
        {
            if (UPipouCharacterStateWalk* WalkState = Pipou->FindComponentByClass<UPipouCharacterStateWalk>())
            {
                WalkState->SetFollowEffectActive(bShouldEffectBeOn);
            }
        }

        if (bIsMovingNow)
        {
            bAnyPlayerMoving = true;
        }
    }
}

#pragma endregion

void UAC_SkeletonFollower::OnParentHit(AActor* SelfActor, AActor* OtherActor,
    FVector NormalImpulse, const FHitResult& Hit)
{
    if (!OtherActor) return;

    bool bIsVillageBorder = OtherActor->ActorHasTag("VillageBorder");

    if (!bIsVillageBorder)
    {
        if (UPrimitiveComponent* OtherRoot = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
        {
            if (OtherRoot->GetCollisionObjectType() == ECC_GameTraceChannel4)
            {
                bIsVillageBorder = true;
            }
        }
    }

    // On enter village
    if (bIsVillageBorder)
    {   
        bCanFollowPlayers = false;
        bStartFollowing = false;

        OnEnterVillage.Broadcast();
    }
}

void UAC_SkeletonFollower::OnParentOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
    if (!OtherActor) return;

    bool bIsVillageBorder = OtherActor->ActorHasTag("VillageBorder");

    if (!bIsVillageBorder)
    {
        if (UPrimitiveComponent* OtherRoot = Cast<UPrimitiveComponent>(OtherActor->GetRootComponent()))
        {
            const ECollisionChannel Channel = OtherRoot->GetCollisionObjectType();
            bIsVillageBorder = (Channel == ECC_GameTraceChannel4);
        }
    }

    if (!bIsVillageBorder)
        return;

    // On enter village
    UE_LOG(LogTemp, Warning, TEXT("[SkeletonFollower] VillageBorder overlap detected with %s"),
        *GetNameSafe(OtherActor));

    bCanFollowPlayers = false;
    bStartFollowing = false;

    OnEnterVillage.Broadcast();

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

    USplineComponent* VillageSpline = nullptr;

    if (UWorld* World = GetWorld())
    {
        if (AActor* ManagerActor = UGameplayStatics::GetActorOfClass(World, AVillagePathManager::StaticClass()))
        {
            if (AVillagePathManager* Manager = Cast<AVillagePathManager>(ManagerActor))
            {
                VillageSpline = Manager->GetSplineForSkeleton(ParentActor);
            }
        }
    }

    if (!VillageSpline)
    {
        VillageSpline = FindNearestSplineToOwner(true);
    }

    if (VillageSpline)
    {
        SplineToFollow = VillageSpline;

        bOnVillageSpline = true;
        bHasReachedHome = false;

        StartFollowingSplineFromClosestPoint(true);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SkeletonFollower: No village spline found/configured for %s."),
            *GetNameSafe(ParentActor));
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

bool UAC_SkeletonFollower::TrySnapToGround(const FVector& In, FVector& Out) const
{
    UWorld* World = GetWorld();
    if (!World) return false;

    const FVector Start = In + FVector(0, 0, GroundTraceUp);
    const FVector End = In - FVector(0, 0, GroundTraceDown);

    FHitResult Hit;
    FCollisionQueryParams Params(SCENE_QUERY_STAT(SnapToGround), false, GetOwner());

    bool bHit = World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    if (bHit)
    {
        Out = In;
        Out.Z = Hit.ImpactPoint.Z + GroundOffset;
        return true;
    }
    return false;
}


//village

void UAC_SkeletonFollower::HandleReachHome()
{
    UE_LOG(LogTemp, Log, TEXT("[SkeletonFollower] HandleReachHome called"));

    if (!ParentActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SkeletonFollower] ParentActor is null"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SkeletonFollower] World is null"));
        return;
    }

    // Find the VillagePathManager in the level
    AActor* ManagerActor = UGameplayStatics::GetActorOfClass(World, AVillagePathManager::StaticClass());
    if (!ManagerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("[SkeletonFollower] No VillagePathManager found in world"));
        return;
    }

    if (AVillagePathManager* Manager = Cast<AVillagePathManager>(ManagerActor))
    {
        UE_LOG(LogTemp, Log, TEXT("[SkeletonFollower] Notifying VillagePathManager for %s"),
            *GetNameSafe(ParentActor));

        Manager->OnSkeletonReachedEnd(ParentActor);
    }
}

void UAC_SkeletonFollower::ResumeFollowingSpline()
{
    bFollowingSpline = true;
    bOnVillageSpline = true;
    bHasReachedHome = false;

    UE_LOG(LogTemp, Log, TEXT("[SkeletonFollower] Dialogue finished, resuming spline follow."));
}

/*
FollowerComponent->OnWaitingForDialogue.AddDynamic(this, &AMyDialogueManager::StartDialogue);
*/

//debug

void UAC_SkeletonFollower::ClearAllDebugLines()
{
    if (UWorld* World = GetWorld())
    {
        FlushPersistentDebugLines(World);
        FlushDebugStrings(World);
    }
}
