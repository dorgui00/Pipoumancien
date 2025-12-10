// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/SkeletonController.h"

#include "Character/PipouCharacter.h"
#include "Data/GlobalDataTableSubsystem.h"
#include "Game/GlobalGameSubsystem.h"
#include "PNJ/AC_SkeletonFollower.h"
#include "UI/UIDialoge.h"
#include "Components/SkeletalMeshComponent.h"
#include "Animation/AnimationAsset.h"
#include "BP/Fog.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "DrawDebugHelpers.h"
#include "MyAnimNotify_PlayCleanseOnce.h"
#include "Data/FSkeletonVisuals.h"


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

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonController::ASkeletonController::BeginOverlaps);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASkeletonController::EndOverlaps);

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

	// ---- SET MYSELF ----
	MySkeleton = GetGameInstance()->GetSubsystem<UGlobalDataTableSubsystem>()->GetSkeletonByID(ID);
	
	// Anims
	if (!TargetMesh)
	{
		TargetMesh = FindComponentByClass<USkeletalMeshComponent>();
	}
	
	InitMyVisuals(); 

	// Start with idle if we have it
	if (TargetMesh && IdleAnimation)
	{
		TargetMesh->PlayAnimation(IdleAnimation, true);
	}

	LastLocation = GetActorLocation();
	bHasLastLocation = true;
	bWasMoving = false;
	bWasFollowing = false;

	// UI
	WidgetComponent->SetVisibility(false);
}

void ASkeletonController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	SphereComponent->OnComponentBeginOverlap.RemoveDynamic(this, &ASkeletonController::BeginOverlaps);
	SphereComponent->OnComponentEndOverlap.RemoveDynamic(this, &ASkeletonController::EndOverlaps);
}

// Called every frame
void ASkeletonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateAnimation(DeltaTime);

}

void ASkeletonController::BeginOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		if (MyState == ESkeletonState::Dialogue)
		{
			InterationDialogue();
		}
	}
}

void ASkeletonController::EndOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		if (MyState == ESkeletonState::Dialogue)
		{
			WidgetComponent->SetVisibility(false);
		}
	}
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

	
	isDialogVisible = true;
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
	WidgetComponent->SetVisibility(false);
	PlayerWidget = CreateWidget<UUIDialoge>(GetWorld(), PlayerWidgetClass);
	PlayerWidget->SetDialogue(MySkeleton,ValutFrase);
	if (ValutFrase == 0)
	{
		ValutFrase = 1;	
	}
}

void ASkeletonController::InterationDialogue()
{
	if (!WidgetComponent->IsVisible())
	{
		WidgetComponent->SetVisibility(true);			
	}
}

void ASkeletonController::InterationDialoguenOFF()
{
		WidgetComponent->SetVisibility(false);
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

void ASkeletonController::InitMyVisuals()
{
	if (!MySkeleton) return;
	if (!MySkeleton->SkeletonVisuals.SkeletonMesh) return;
	
	 // -- MESH --
	TargetMesh->SetSkeletalMeshAsset(MySkeleton->SkeletonVisuals.SkeletonMesh) ;

	// -- ANIMS --
	
	//wake
	WakeAnimation = MySkeleton->SkeletonVisuals.WakeAnim;
	if (!WakeAnimation)
		UE_LOG(LogTemp,Error,TEXT("Wake Anim is null"));
	
	// transport
	IdleAnimation = MySkeleton->SkeletonVisuals.IdleAnim;
	if (!IdleAnimation)
		UE_LOG(LogTemp,Error,TEXT("Idle Anim is null"));
	
	WalkAnimation = MySkeleton->SkeletonVisuals.WalkAnim;
	if (!WalkAnimation)
		UE_LOG(LogTemp,Error,TEXT("Walk Anim is null"));
	
	WaitAnimation = MySkeleton->SkeletonVisuals.WaitAnim;
	if (!WaitAnimation)
	{
		UE_LOG(LogTemp,Warning,TEXT("Wait Anim is null"));
		if (IdleAnimation) WaitAnimation = IdleAnimation;
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

//NIAGARA SFX

void ASkeletonController::StartCleanseWindow()
{
	if (bCleanseFXPlayed)
		return;

	bCleanseWindowActive = true;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			CleanseWindowTimerHandle,
			this,
			&ASkeletonController::EndCleanseWindow,
			CleanseWindowDuration,
			false
		);
	}
}

void ASkeletonController::EndCleanseWindow()
{
	bCleanseWindowActive = false;
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



