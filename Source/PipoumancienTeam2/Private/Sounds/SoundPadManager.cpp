// Fill out your copyright notice in the Description page of Project Settings.


#include "Sounds/SoundPadManager.h"

#include "Components/AudioComponent.h"
#include "Components/SceneComponent.h"
#include "Engine/TriggerBox.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"

ASoundPadManager::ASoundPadManager()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	AudioA = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioA"));
	AudioA->SetupAttachment(RootComponent);
	AudioA->bAutoActivate = false;

	AudioB = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioB"));
	AudioB->SetupAttachment(RootComponent);
	AudioB->bAutoActivate = false;

	bNecroInVillage = false;
	bPhantomInVillage = false;
}

void ASoundPadManager::BeginPlay()
{
	Super::BeginPlay();

	if (VillageTrigger)
	{
		VillageTrigger->OnActorBeginOverlap.AddDynamic(this, &ASoundPadManager::OnVillageBeginOverlap);
		VillageTrigger->OnActorEndOverlap.AddDynamic(this, &ASoundPadManager::OnVillageEndOverlap);
	}

	NecroPlayer = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), NecroCharacterClass));
	PhantomPlayer = Cast<ACharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), PhantomCharacterClass));

	if (OutsidePad)
	{
		AudioA->SetSound(OutsidePad);
		AudioA->Play();
		AudioA->SetVolumeMultiplier(1.f);
	}
}

void ASoundPadManager::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsCrossfading)
	{
		CrossfadeTimer += DeltaSeconds;
		float Alpha = FMath::Clamp(CrossfadeTimer / CrossfadeDuration, 0.f, 1.f);

		if (FadingInAudio)
			FadingInAudio->SetVolumeMultiplier(Alpha);

		if (FadingOutAudio)
			FadingOutAudio->SetVolumeMultiplier(1.f - Alpha);

		if (Alpha >= 1.f)
		{
			// Crossfade complete
			if (FadingOutAudio)
			{
				FadingOutAudio->Stop();
				FadingOutAudio->SetVolumeMultiplier(0.f);
			}
			bIsCrossfading = false;
			FadingInAudio = nullptr;
			FadingOutAudio = nullptr;
			CrossfadeTimer = 0.f;
		}
	}
}

void ASoundPadManager::OnVillageBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == NecroPlayer)
		bNecroInVillage = true;
	else if (OtherActor == PhantomPlayer)
		bPhantomInVillage = true;

	UpdatePad();
}

void ASoundPadManager::OnVillageEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (OtherActor == NecroPlayer)
		bNecroInVillage = false;
	else if (OtherActor == PhantomPlayer)
		bPhantomInVillage = false;

	UpdatePad();
}

void ASoundPadManager::UpdatePad()
{
	if (bNecroInVillage && bPhantomInVillage)
	{
		StartCrossfade(VillagePad);
	}
	else
	{
		StartCrossfade(OutsidePad);
	}
}

void ASoundPadManager::StartCrossfade(USoundBase* NewPad)
{
	if (!NewPad)
		return;

	UAudioComponent* ActiveAudio = nullptr;
	UAudioComponent* InactiveAudio = nullptr;

	if (AudioA->IsPlaying())
		ActiveAudio = AudioA;
	else if (AudioB->IsPlaying())
		ActiveAudio = AudioB;

	InactiveAudio = (ActiveAudio == AudioA) ? AudioB : AudioA;

	if (ActiveAudio && ActiveAudio->Sound == NewPad)
		return;

	InactiveAudio->SetSound(NewPad);
	InactiveAudio->Play();
	InactiveAudio->SetVolumeMultiplier(0.f);

	// Begin crossfade
	FadingInAudio = InactiveAudio;
	FadingOutAudio = ActiveAudio;
	CrossfadeTimer = 0.f;
	bIsCrossfading = true;
}