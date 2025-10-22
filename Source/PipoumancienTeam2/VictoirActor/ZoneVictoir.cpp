// Fill out your copyright notice in the Description page of Project Settings.


#include "ZoneVictoir.h"

#include "Blueprint/WidgetBlueprintGeneratedClass.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#

// Sets default values
AZoneVictoir::AZoneVictoir()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ZoneVictoirMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ZoneVictoirMesh->SetupAttachment(RootComponent);
	
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(100.00f);
}

// Called when the game starts or when spawned
void AZoneVictoir::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AZoneVictoir::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AZoneVictoir::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && (OtherActor != this) && OtherComp) 
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Overlap Begin"));
	}
}



