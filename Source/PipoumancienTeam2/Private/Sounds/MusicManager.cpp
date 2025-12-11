// Fill out your copyright notice in the Description page of Project Settings.


#include "Sounds/MusicManager.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

// Sets default values
AMusicManager::AMusicManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

    MainMusicAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MainMusicAudio"));
    MainMusicAudioComp->SetupAttachment(RootComponent);
    MainMusicAudioComp->bAutoActivate = false;
    MainMusicAudioComp->bIsUISound = true;

    VillageMusicAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("VillageMusicAudio"));
    VillageMusicAudioComp->SetupAttachment(RootComponent);
    VillageMusicAudioComp->bAutoActivate = false;
    VillageMusicAudioComp->bIsUISound = true;
}

// Called when the game starts or when spawned
void AMusicManager::BeginPlay()
{
	Super::BeginPlay();

    if (MainMusicSound)
    {
        MainMusicAudioComp->SetSound(MainMusicSound);
    }

    if (VillageMusicSound)
    {
        VillageMusicAudioComp->SetSound(VillageMusicSound);
    }

    BindToTrigger();

    if (bStartWithMainMusic && MainMusicSound)
    {
        MainMusicAudioComp->FadeIn(MainFadeInTime, 1.0f, 0.0f);
    }
}

// Called every frame
void AMusicManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMusicManager::BindToTrigger()
{
    if (!VillageTriggerActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("MusicManager: VillageTriggerActor not set."));
        return;
    }

    TArray<UPrimitiveComponent*> PrimitiveComponents;
    VillageTriggerActor->GetComponents<UPrimitiveComponent>(PrimitiveComponents);

    bool bBound = false;

    for (UPrimitiveComponent* PrimComp : PrimitiveComponents)
    {
        if (!PrimComp)
        {
            continue;
        }

        if (PrimComp->GetGenerateOverlapEvents())
        {
            PrimComp->OnComponentBeginOverlap.AddDynamic(this, &AMusicManager::OnTriggerBeginOverlap);
            PrimComp->OnComponentEndOverlap.AddDynamic(this, &AMusicManager::OnTriggerEndOverlap);

            bBound = true;
            break;
        }
    }

    if (!bBound)
    {
        UE_LOG(LogTemp, Warning, TEXT("MusicManager: No overlap-generating primitive components found on %s"),
            *VillageTriggerActor->GetName());
    }
}

void AMusicManager::OnTriggerBeginOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (!OtherActor->IsA(APipouCharacter::StaticClass()))
    {
        return;
    }

    if (NumPlayersInTrigger == 0)
    {
        EnterVillageMusic();
    }

    ++NumPlayersInTrigger;
}

void AMusicManager::OnTriggerEndOverlap(
    UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex
)
{
    if (!OtherActor || OtherActor == this)
    {
        return;
    }

    if (!OtherActor->IsA(APipouCharacter::StaticClass()))
    {
        return;
    }

    NumPlayersInTrigger = FMath::Max(NumPlayersInTrigger - 1, 0);

    if (NumPlayersInTrigger == 0)
    {
        ReturnToMainMusic();
    }
}



//fade in fade out

void AMusicManager::EnterVillageMusic()
{
    if (MainMusicAudioComp && MainMusicAudioComp->IsPlaying())
    {
        MainMusicAudioComp->FadeOut(MainFadeOutTime, 0.0f);
    }

    if (VillageMusicAudioComp && VillageMusicSound)
    {
        if (!VillageMusicAudioComp->IsPlaying())
        {
            VillageMusicAudioComp->FadeIn(VillageFadeInTime, 1.0f, 0.0f);
        }
        else
        {
            VillageMusicAudioComp->SetVolumeMultiplier(1.0f);
        }
    }
}

void AMusicManager::ReturnToMainMusic()
{
    if (VillageMusicAudioComp && VillageMusicAudioComp->IsPlaying())
    {
        VillageMusicAudioComp->FadeOut(VillageFadeOutTime, 0.0f);
    }

    if (MainMusicAudioComp && MainMusicSound)
    {
        if (!MainMusicAudioComp->IsPlaying())
        {
            MainMusicAudioComp->FadeIn(MainFadeInTime, 1.0f, 0.0f);
        }
        else
        {
            MainMusicAudioComp->SetVolumeMultiplier(1.0f);
        }
    }
}