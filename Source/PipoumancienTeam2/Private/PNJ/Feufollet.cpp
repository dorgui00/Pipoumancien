// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/Feufollet.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "Engine/Engine.h"


// Sets default values
AFeufollet::AFeufollet()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetMobility(EComponentMobility::Movable);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(100.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionObjectType(ECC_WorldDynamic);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
}

// Called when the game starts or when spawned
void AFeufollet::BeginPlay()
{
	Super::BeginPlay();

	if (CollisionSphere)
	{
		CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFeufollet::OnOverlapBegin);
	}

	if (Mesh && Mesh->GetMaterial(0))
	{
		DynamicMaterial = Mesh->CreateAndSetMaterialInstanceDynamic(0);
		if (DynamicMaterial && OpacityParameterName != NAME_None)
		{
			DynamicMaterial->SetScalarParameterValue(OpacityParameterName, 1.0f);
		}
	}
	
}

// Called every frame
void AFeufollet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsFleeing)
	{
		if (FleeDirection.IsNearlyZero())
		{
			return;
		}

		const FVector Current = GetActorLocation();
		const FVector DeltaMove = FleeDirection * FleeSpeed * DeltaTime;
		const FVector NewLocation = Current + DeltaMove;

		AddActorWorldOffset(DeltaMove, false);

		BobTime += DeltaTime;

		const float BobOffset = FMath::Sin(BobTime * BobFrequency * 2.f * PI) * BobAmplitude;

		const float DeltaBob = BobOffset - LastBobOffset;
		LastBobOffset = BobOffset;

		AddActorWorldOffset(FVector(0.f, 0.f, DeltaBob), false);
	}

	if (bIsFading && DynamicMaterial && FadeDuration > 0.0f)
	{
		FadeElapsedTime += DeltaTime;
		const float Alpha = FMath::Clamp(FadeElapsedTime / FadeDuration, 0.0f, 1.0f);

		const float CurrentOpacity = FMath::Lerp(1.0f, 0.0f, Alpha);
		DynamicMaterial->SetScalarParameterValue(OpacityParameterName, CurrentOpacity);

		if (Alpha >= 1.0f)
		{
			Destroy();
		}
	}
}

void AFeufollet::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!OtherActor || OtherActor == this || bIsFleeing)
	{
		return;
	}

	ACharacter* OverlappedCharacter = Cast<ACharacter>(OtherActor);
	if (!OverlappedCharacter)
	{
		return;
	}


	const bool bIsPhantom = PhantomCharacterClass && OtherActor->IsA(PhantomCharacterClass);
	const bool bIsNecro = NecroCharacterClass && OtherActor->IsA(NecroCharacterClass);

	if (!bIsPhantom && !bIsNecro)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("[Feufollet] colliding with something"));

	const FVector MyLocation = GetActorLocation();
	const FVector PlayerLocation = OverlappedCharacter->GetActorLocation();
	FleeDirection = (MyLocation - PlayerLocation).GetSafeNormal();

	if (FleeDirection.IsNearlyZero())
	{
		FleeDirection = FVector(1, 0, 0);
	}

	bIsFleeing = true;
	BobTime = 0.f;
	LastBobOffset = 0.f;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			FadeStartTimerHandle,
			this,
			&AFeufollet::StartFade,
			FadeDelay,
			false
		);
	}
}


void AFeufollet::StartFade()
{
	bIsFading = true;
	FadeElapsedTime = 0.0f;

	if (DynamicMaterial && OpacityParameterName != NAME_None)
	{
		DynamicMaterial->SetScalarParameterValue(OpacityParameterName, 1.0f);
	}
}

