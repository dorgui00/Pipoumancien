// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/SkeletonController.h"

#include "Character/PipouCharacter.h"
#include "Data/GlobalDataTableSubsystem.h"
#include "Game/GlobalGameSubsystem.h"
#include "PNJ/AC_SkeletonFollower.h"
#include "PNJ/AC_SetAnimations.h"
#include "UI/UIDialoge.h"


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
}

// Called every frame
void ASkeletonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	
	
	// ANIMS
	AnimComponent = Cast<UAC_SetAnimations>(AddComponentByClass(UAC_SetAnimations::StaticClass(), true, GetTransform(), false));

	if (AnimComponent)
	{
		AnimComponent->SetReference(TargetMesh);

		AnimComponent->SetAnimations(IdleAnim, WalkAnim, WaitAnim);

		UE_LOG(LogTemp, Display, TEXT("mama mia"));


	} else {
	
		UE_LOG(LogTemp, Display, TEXT("ALEEEEEED"));
	}
}

void ASkeletonController::OnEnterVillage()
{
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
