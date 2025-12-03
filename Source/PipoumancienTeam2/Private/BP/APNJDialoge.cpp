// Fill out your copyright notice in the Description page of Project Settings.


#include "BP/APNJDialoge.h"
#include "Character/PipouCharacter.h"


// Sets default values
AAPNJDialoge::AAPNJDialoge()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	ZoneVictoirMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	ZoneVictoirMesh->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(1000);
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AAPNJDialoge::AAPNJDialoge::BeginOverlaps);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AAPNJDialoge::endOverlaps);

	//HUD
	//PlayerHUDClass = nullptr;
	//PlayerHUD = nullptr;
}

// Called when the game starts or when spawned
void AAPNJDialoge::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAPNJDialoge::BeginOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		//if (PlayerHUDClass && PlayerHUD == nullptr)
		//{
		//	PlayerHUD = CreateWidget<UUserWidget>(GetWorld(), PlayerHUDClass);
		//	if (PlayerHUD)
		//	{
		//		PlayerHUD->AddToViewport();
		//		UE_LOG(LogTemp, Warning, TEXT("HUD Affiché"));
		//	}
		//}
	}
}

void AAPNJDialoge::endOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->IsA(APipouCharacter::StaticClass()))
	{
		
	}
}

// Called every frame
void AAPNJDialoge::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



