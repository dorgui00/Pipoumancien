// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/AssignActorComponent.h"
#include "BushVFX.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/SceneComponent.h"

// Sets default values
AAssignActorComponent::AAssignActorComponent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAssignActorComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AAssignActorComponent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAssignActorComponent::ScanForActor()
{
	Actors.Empty();

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FString Prefix = ActorNamePrefix;

	for (TActorIterator<AActor> It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (!IsValid(Actor))
		{
			continue;
		}

		const FString Id = GetActorIdForMatching(Actor);
		if (!Id.StartsWith(Prefix))
		{
			continue;
		}

		if (USceneComponent* Root = Actor->GetRootComponent())
		{
#if WITH_EDITOR
			if (GIsEditor)
			{
				Root->Modify();
			}
#endif
			Root->SetMobility(EComponentMobility::Movable);
		}

		FActorsData Data;
		Data.Actor = Actor;

		Actors.Add(Data);
	}
}

void AAssignActorComponent::AssignComponents()
{
#if WITH_EDITOR
    FScopedTransaction Transaction(
        NSLOCTEXT("AssignActorComponent", "AssignComponents", "Assign Bush VFX Components"));
#endif

    UE_LOG(LogTemp, Log, TEXT("AssignComponents called. Num Actors: %d"), Actors.Num());

    for (const FActorsData& Data : Actors)
    {
        if (!Data.Actor || !ComponentToAssign)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("Skipping entry: Actor or ComponentToAssign is null"));
            continue;
        }

#if WITH_EDITOR
        Data.Actor->Modify();
#endif

        bool bAlreadyHas = false;
        for (UActorComponent* ExistingComp : Data.Actor->GetComponents())
        {
            if (ExistingComp && ExistingComp->GetClass() == ComponentToAssign)
            {
                UE_LOG(LogTemp, Log,
                    TEXT("%s already has %s"),
                    *Data.Actor->GetName(),
                    *ComponentToAssign->GetName());
                bAlreadyHas = true;
                break;
            }
        }
        if (bAlreadyHas)
            continue;

        UBushVFX* NewBushVFX = NewObject<UBushVFX>(
            Data.Actor,
            ComponentToAssign,
            NAME_None,
            RF_Transactional
        );

        if (!NewBushVFX)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("Failed to create BushVFX for %s"),
                *Data.Actor->GetName());
            continue;
        }

        NewBushVFX->InitializeFromTool(
            FXToAssign,
            PhantomClassToAssign,
            NecroClassToAssign
        );

        Data.Actor->AddInstanceComponent(NewBushVFX);

        NewBushVFX->OnComponentCreated();
        NewBushVFX->RegisterComponent();

#if WITH_EDITOR
        Data.Actor->PostEditChange();
#endif

        UE_LOG(LogTemp, Log,
            TEXT("Added BushVFX component to %s"),
            *Data.Actor->GetName());
    }

}FString AAssignActorComponent::GetActorIdForMatching(AActor* Actor) const
{
	if (!Actor)
	{
		return FString();
	}

#if WITH_EDITOR
	return Actor->GetActorLabel();
#else
	return Actor->GetName();
#endif
}