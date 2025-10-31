
#include "PNJ/AC_SkeletonFollower.h"

#include "Character/PipouCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

UAC_SkeletonFollower::UAC_SkeletonFollower()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAC_SkeletonFollower::BeginPlay()
{
	Super::BeginPlay();

#pragma region Find the stuff / init

	//find players
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), PlayersTag, PipouPlayers);
	//find parent actor
	ParentActor = GetOwner();

#pragma endregion

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
		return;
	}

	CheckPlayerRange();
	FollowPlayers(DeltaTime);
}

#pragma region Player Follow

void UAC_SkeletonFollower::CheckPlayerRange()
{
	if (bStartFollowing) 
	{
		return;

	} else {

		FVector ParentLocation = ParentActor->GetActorLocation();

		for (int32 i = 0; i < PipouPlayers.Num(); i++)
		{
			if (AActor* Player = PipouPlayers[i])
			{
				float Distance = FVector::Dist(Player->GetActorLocation(), ParentLocation);

				if (Distance <= FollowRange)
				{
					if (i == 0) bPlayer1InRange = true;
					if (i == 1) bPlayer2InRange = true;
				}
			}
		}

		if (bPlayer1InRange && bPlayer2InRange)
		{
			bStartFollowing = true;
		}
	}

}

void UAC_SkeletonFollower::FollowPlayers(float DeltaTime) //tick function
{
	if (bStartFollowing)
	{
		if (ParentActor && PipouPlayers.Num() >= 2)
		{
			Player1Location = PipouPlayers[0]->GetActorLocation();
			Player2Location = PipouPlayers[1]->GetActorLocation();
			FVector TargetLocation = (Player1Location + Player2Location) / 2;

			FVector CurrentLocation = ParentActor->GetActorLocation();
			FVector NewLocation = FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, MoveSpeed);

			ParentActor->SetActorLocation(NewLocation);
		}
	}
}

#pragma endregion

#pragma region OnParentHit/OnParentOverlap

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
		// Stop the player-centroid following
		bStartFollowing = false;

		// Pick/confirm a spline
		if (!SplineToFollow)
		{
			SplineToFollow = FindNearestSplineToOwner();
		}

		if (SplineToFollow)
		{
			StartFollowingSplineFromClosestPoint();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("SkeletonFollower: No spline found near %s."), *GetOwner()->GetName());
		}
	}
}

USplineComponent* UAC_SkeletonFollower::FindNearestSplineToOwner() const
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

		TInlineComponentArray<USplineComponent*> Splines(A);
		for (USplineComponent* Spline : Splines)
		{
			if (!Spline) continue;

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

	if (CurrentDistance >= TargetDistance - KINDA_SMALL_NUMBER)
	{
		bFollowingSpline = false;

		const FVector EndLoc = SplineToFollow->GetLocationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
		if (bOrientToSpline)
		{
			const FRotator EndRot = SplineToFollow->GetRotationAtDistanceAlongSpline(TargetDistance, ESplineCoordinateSpace::World);
			ParentActor->SetActorLocationAndRotation(EndLoc, EndRot);
		}
		else
		{
			ParentActor->SetActorLocation(EndLoc);
		}

		DestroyComponent();
	}
}

#pragma endregion

#pragma region Debug Lines

void UAC_SkeletonFollower::ClearAllDebugLines()
{
	if (UWorld* World = GetWorld())
	{
		FlushPersistentDebugLines(World);
		FlushDebugStrings(World);
	}
}

#pragma endregion

