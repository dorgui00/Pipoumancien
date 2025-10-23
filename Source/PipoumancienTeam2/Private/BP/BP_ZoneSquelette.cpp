// Fill out your copyright notice in the Description page of Project Settings.


#include "BP/BP_ZoneSquelette.h"

#include "Character/PipouCharacter.h"


// Sets default values
ABP_ZoneSquelette::ABP_ZoneSquelette()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ZoneVictoirMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ZoneVictoirMesh->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(1000);
	

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABP_ZoneSquelette::ABP_ZoneSquelette::BeginOverlaps);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABP_ZoneSquelette::endOverlaps);

	//HUD
	PlayerHUDClass = nullptr;
	PlayerHUD = nullptr;
}

// Called when the game starts or when spawned
void ABP_ZoneSquelette::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABP_ZoneSquelette::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABP_ZoneSquelette::BeginOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
	//	APipouCharacter* FoundActor = Cast<APipouCharacter>(OtherActor);
	//	FoundActor -> HUDPlay();
		
	}
}

void ABP_ZoneSquelette::endOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
	//	APipouCharacter* FoundActor = Cast<APipouCharacter>(OtherActor);
	//	FoundActor-> HUDExit();
	
		
	}
}
