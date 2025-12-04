// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/SkeletonController.h"

#include "Character/PipouCharacter.h"
#include "Data/GlobalDataTableSubsystem.h"
#include "Game/GlobalGameSubsystem.h"
#include "PNJ/AC_SkeletonFollower.h"
#include "PNJ/AC_SetAnimations.h"
#include "UI/UIDialoge.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "BP/Fog.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DrawDebugHelpers.h"



// Sets default values
ASkeletonController::ASkeletonController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ZoneVictoirMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ZoneVictoirMesh->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(500);
	
	//SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonController::ASkeletonController::BeginOverlaps);
	//SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASkeletonController::EndOverlaps);

	//HUD
	PlayerWidgetClass = nullptr;
	PlayerWidget = nullptr;
}

ASkeletonController::~ASkeletonController()
{
	// REMOVE FOLLOW
	if (FollowComponent)
	{
		FollowComponent->OnEnterVillage.RemoveDynamic(this, &ASkeletonController::OnEnterVillage);
		FollowComponent->OnReachHome.RemoveDynamic(this, &ASkeletonController::OnReachHome);
	}
}

// Called when the game starts or when spawned
void ASkeletonController::BeginPlay()
{
	Super::BeginPlay();

	MySkeleton = GetGameInstance()->GetSubsystem<UGlobalDataTableSubsystem>()->GetSkeletonByID(ID);

	//ANIM
	if (!TargetMesh)
	{
		TargetMesh = FindComponentByClass<USkeletalMeshComponent>();
	}

	// Start with idle if we have it
	if (TargetMesh && IdleAnimation)
	{
		TargetMesh->PlayAnimation(IdleAnimation, true);
	}

	LastLocation = GetActorLocation();
	bHasLastLocation = true;
	bWasMoving = false;
	bWasFollowing = false;
}

// Called every frame
void ASkeletonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimation(DeltaTime);

}

void ASkeletonController::BeginOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}

void ASkeletonController::EndOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

// STATE
ESkeletonState ASkeletonController::GetState() const
{
	return MyState;
}

void ASkeletonController::SetSkeletonForTransport()
{
	// STATE
	MyState = ESkeletonState::Transport;
	
	//ADD FOLLOW
	FollowComponent = Cast<UAC_SkeletonFollower>(AddComponentByClass(UAC_SkeletonFollower::StaticClass(), true, GetTransform(), false));

	if (FollowComponent)
	{
		FollowComponent->OnEnterVillage.AddDynamic(this, &ASkeletonController::OnEnterVillage);
		FollowComponent->OnReachHome.AddDynamic(this, &ASkeletonController::OnReachHome);
	}
	

}

void ASkeletonController::OnEnterVillage()
{
	FogDilet();
	// MY STATE
	MyState = ESkeletonState::BackToHome;

	// WORLD STATE : Free
	if (UGlobalGameSubsystem* GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>())
		GlobalGameSubsystem->SetWorldFreeState();
}


void ASkeletonController::SetSkeletonForDialogue()
{
	// MY STATE
	MyState = ESkeletonState::Dialogue;
}

void ASkeletonController::OnReachHome()
{
	SetSkeletonForDialogue();
}

void ASkeletonController::OpenDialogue()
{
	PlayerWidget = CreateWidget<UUIDialoge>(GetWorld(), PlayerWidgetClass);
	PlayerWidget->SetDialogue(MySkeleton,ValutFrase);
	if (ValutFrase == 0)
	{
		ValutFrase = 1;	
	}
}

//ANIMATION

void ASkeletonController::UpdateAnimation(float DeltaTime)
{
	// no mesh, no animation / footsteps
	if (!TargetMesh)
	{
		TargetMesh = FindComponentByClass<USkeletalMeshComponent>();
		if (!TargetMesh)
		{
			return;
		}
	}

	// check if following the player
	bool bIsFollowing = false;
	if (FollowComponent)
	{
		bIsFollowing = FollowComponent->IsStartFollowing();
	}

	const FVector CurrentLocation = GetActorLocation();
	bool bIsMoving = false;

	// movement detection
	if (!bHasLastLocation)
	{
		LastLocation = CurrentLocation;
		bHasLastLocation = true;
	}
	else
	{
		const float DistanceMovedSq = FVector::DistSquared(CurrentLocation, LastLocation);
		bIsMoving = DistanceMovedSq > 1.0f; // small threshold
	}

	// --------- ANIMATION STATE ---------
	if (bIsMoving != bWasMoving || bIsFollowing != bWasFollowing)
	{
		if (bIsFollowing)
		{
			if (!bIsMoving)
			{
				PlayWait();
			}
			else
			{
				PlayWalk();
			}
		}
		else
		{
			if (bIsMoving)
			{
				PlayWalk();
			}
			else
			{
				PlayIdle();
			}
		}

		bWasMoving = bIsMoving;
		bWasFollowing = bIsFollowing;
	}

	// uadio
	if (bIsMoving)
	{
		FootstepTimer += DeltaTime;

		if (FootstepTimer >= FootstepInterval)
		{
			FootstepTimer = 0.f;

			FHitResult Hit;
			FCollisionQueryParams Params;
			Params.bReturnPhysicalMaterial = true;

			const FVector BaseLoc = (TargetMesh ? TargetMesh->GetComponentLocation() : GetActorLocation());

			FVector Start = BaseLoc + FVector(0.f, 0.f, 100.f);
			FVector End = BaseLoc - FVector(0.f, 0.f, 1000.f);

			bool bHit = GetWorld()->LineTraceSingleByChannel(
				Hit,
				Start,
				End,
				ECC_Visibility,
				Params
			);

			if (!bHit)
			{
				UE_LOG(LogTemp, Warning, TEXT("[SkeletonController] step trace hit NOTHIIIIIIING HEEEEEELP MEEEEEEEEEEEE"));
			}
			else
			{
				AActor* HitActor = Hit.GetActor();
				UPrimitiveComponent* HitComp = Hit.GetComponent();
				UPhysicalMaterial* PhysMat = Hit.PhysMaterial.Get();

				if (PhysMat)
				{
					PlayFootstepsSound(PhysMat);
				}
			}
		}
	}
	else
	{
		// standing still so reset timer
		FootstepTimer = 0.f;
	}

	LastLocation = CurrentLocation;
}


//ANIMATIONS

void ASkeletonController::FogDilet()
{
	for (AFog* Fog : FogList)
	{
		if (Fog)
		{
			Fog->SupprimerFog(MySkeleton);  // Appel sur chaque élément
		}
	}
}

void ASkeletonController::PlayIdle()
{
	if (TargetMesh && IdleAnimation)
	{
		TargetMesh->PlayAnimation(IdleAnimation, true);
	}
}

void ASkeletonController::PlayWalk()
{
	if (TargetMesh && WalkAnimation)
	{
		TargetMesh->PlayAnimation(WalkAnimation, true);
	}
}

void ASkeletonController::PlayWait()
{
	if (TargetMesh && WaitAnimation)
	{
		TargetMesh->PlayAnimation(WaitAnimation, true);
	}
}

// ------------------ //

void ASkeletonController::PlayFootstepsSound(UPhysicalMaterial* PhysMat)
{
	if (!FootstepCue)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Skeleton] stepcue not set"));
		return;
	}

	if (!PhysMat)
	{
		return;
	}

	const FVector Location = GetActorLocation();

	UAudioComponent* AudioComp = UGameplayStatics::SpawnSoundAtLocation(
		this,
		FootstepCue,
		Location
	);

	if (!AudioComp)
	{
		return;
	}

	const int32 SurfaceIndex = static_cast<int32>(PhysMat->SurfaceType);
	AudioComp->SetIntParameter(TEXT("SurfaceType"), SurfaceIndex);
}



