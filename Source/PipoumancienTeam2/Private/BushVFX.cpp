// Fill out your copyright notice in the Description page of Project Settings.


#include "BushVFX.h"

#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "Engine/World.h"
#include "TimerManager.h"

UBushVFX::UBushVFX()
{
	PrimaryComponentTick.bCanEverTick = false;

	TriggerSphere = nullptr;
	SphereRadius = 300.f;
	Cooldown = 3.0f;
	bCanTrigger = true;
	EnterFX = nullptr;
}

void UBushVFX::InitializeFromTool(
	UNiagaraSystem* InEnterFX,
	TSubclassOf<AActor> InPhantomClass,
	TSubclassOf<AActor> InNecroClass)
{
	EnterFX = InEnterFX;
	PhantomCharacterClass = InPhantomClass;
	NecroCharacterClass = InNecroClass;
}


void UBushVFX::BeginPlay()
{
	Super::BeginPlay();

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	TriggerSphere = NewObject<USphereComponent>(Owner, TEXT("BushVFX_TriggerSphere"));
	if (!TriggerSphere)
	{
		return;
	}

	TriggerSphere->InitSphereRadius(SphereRadius);
	TriggerSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerSphere->SetCollisionObjectType(ECC_WorldDynamic);
	TriggerSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	TriggerSphere->SetGenerateOverlapEvents(true);

	if (Owner->GetRootComponent())
	{
		TriggerSphere->AttachToComponent(
			Owner->GetRootComponent(),
			FAttachmentTransformRules::KeepRelativeTransform
		);
	}
	else
	{
		Owner->SetRootComponent(TriggerSphere);
	}

	TriggerSphere->RegisterComponent();

	TriggerSphere->OnComponentBeginOverlap.AddDynamic(
		this,
		&UBushVFX::OnSphereBeginOverlap
	);
}

void UBushVFX::OnSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bCanTrigger || !OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	bool bIsValidPlayer = false;

	if (PhantomCharacterClass && OtherActor->IsA(PhantomCharacterClass))
	{
		bIsValidPlayer = true;
	}
	else if (NecroCharacterClass && OtherActor->IsA(NecroCharacterClass))
	{
		bIsValidPlayer = true;
	}

	if (!bIsValidPlayer)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (EnterFX)
	{
		const FVector SpawnLocation = GetOwner()->GetActorLocation();
		const FRotator SpawnRotation = GetOwner()->GetActorRotation();

		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			EnterFX,
			SpawnLocation,
			SpawnRotation
		);
	}

	bCanTrigger = false;
	World->GetTimerManager().SetTimer(
		CooldownTimerHandle,
		this,
		&UBushVFX::ResetCooldown,
		Cooldown,
		false
	);
}

void UBushVFX::ResetCooldown()
{
	bCanTrigger = true;
}