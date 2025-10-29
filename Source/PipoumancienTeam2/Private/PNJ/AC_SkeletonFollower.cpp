// Fill out your copyright notice in the Description page of Project Settings.

#include "PNJ/AC_SkeletonFollower.h"

#include "Character/PipouCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Components/PrimitiveComponent.h"


// Sets default values for this component's properties
UAC_SkeletonFollower::UAC_SkeletonFollower()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
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

	CheckPlayerRange();
	FollowPlayers(DeltaTime);
}

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
		bStartFollowing = false;

		//do stuff here
		//follow spline to village end points?
		//Dorian's UI calls, etc
	}
}

#pragma endregion

