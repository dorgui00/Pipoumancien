// Fill out your copyright notice in the Description page of Project Settings.


#include "Sounds/SoundPadManager.h"

#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/TriggerBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

ASoundPadManager::ASoundPadManager()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false;

	NecroPlayer = nullptr;
	PhantomPlayer = nullptr;
	bNecroInVillage = false;
	bPhantomInVillage = false;
}

void ASoundPadManager::BeginPlay()
{
	Super::BeginPlay();

	if (NecroCharacterClass)
	{
		NecroPlayer = Cast<ACharacter>(
			UGameplayStatics::GetActorOfClass(GetWorld(), *NecroCharacterClass)
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SoundPadManager] NecroCharacterClass not set"));
	}

	if (PhantomCharacterClass)
	{
		PhantomPlayer = Cast<ACharacter>(
			UGameplayStatics::GetActorOfClass(GetWorld(), *PhantomCharacterClass)
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SoundPadManager] PhantomCharacterClass not set"));
	}

	// Bind overlaps on plain trigger
	if (VillageTrigger)
	{
		VillageTrigger->OnActorBeginOverlap.AddDynamic(
			this, &ASoundPadManager::OnVillageBeginOverlap);
		VillageTrigger->OnActorEndOverlap.AddDynamic(
			this, &ASoundPadManager::OnVillageEndOverlap);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("[SoundPadManager] VillageTrigger not set"));
	}

	// Start outside
	if (OutsidePad)
	{
		PlayPad(OutsidePad);
	}
}

void ASoundPadManager::OnVillageBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor)
		return;

	bool bChanged = false;

	if (OtherActor == NecroPlayer)
	{
		bNecroInVillage = true;
		bChanged = true;
	}
	else if (OtherActor == PhantomPlayer)
	{
		bPhantomInVillage = true;
		bChanged = true;
	}

	if (bChanged)
	{
		UpdatePad();
	}
}

void ASoundPadManager::OnVillageEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (!OtherActor)
		return;

	bool bChanged = false;

	if (OtherActor == NecroPlayer)
	{
		bNecroInVillage = false;
		bChanged = true;
	}
	else if (OtherActor == PhantomPlayer)
	{
		bPhantomInVillage = false;
		bChanged = true;
	}

	if (bChanged)
	{
		UpdatePad();
	}
}

void ASoundPadManager::UpdatePad()
{
	// ONLY when both inside do we play the village pad
	if (bNecroInVillage && bPhantomInVillage && VillagePad)
	{
		PlayPad(VillagePad);
	}
	else if (OutsidePad)
	{
		PlayPad(OutsidePad);
	}
}

void ASoundPadManager::PlayPad(USoundBase* NewPad)
{
	if (!AudioComponent || !NewPad)
		return;

	if (AudioComponent->Sound == NewPad && AudioComponent->IsPlaying())
		return;

	AudioComponent->SetSound(NewPad);
	AudioComponent->Play();
}
