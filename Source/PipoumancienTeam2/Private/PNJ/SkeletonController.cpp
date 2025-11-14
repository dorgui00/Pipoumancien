// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/SkeletonController.h"
#include "Character/PipouCharacter.h"
#include "Data/DataTableGameInstanceSubsystem.h"
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
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkeletonController::ASkeletonController::BeginOverlaps);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASkeletonController::endOverlaps);

	//HUD
	PlayerWidgetClass = nullptr;
	PlayerWidget = nullptr;
}

// Called when the game starts or when spawned
void ASkeletonController::BeginPlay()
{
	Super::BeginPlay();

	MySkeleton = GetGameInstance()->GetSubsystem<UDataTableGameInstanceSubsystem>()->GetSkeletonByID(ID);
}

// Called every frame
void ASkeletonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASkeletonController::BeginOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		PlayerWidget = CreateWidget<UUIDialoge>(GetWorld(), PlayerWidgetClass);
		PlayerWidget->SetDialogue(MySkeleton);
	}
}

void ASkeletonController::endOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		if (PlayerWidget)
		{
			PlayerWidget->RemoveFromParent();
			PlayerWidget = nullptr;
			UE_LOG(LogTemp, Warning, TEXT("HUD Supprimé"));
		}
	}
}

