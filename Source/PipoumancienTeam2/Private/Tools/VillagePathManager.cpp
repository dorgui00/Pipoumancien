// Fill out your copyright notice in the Description page of Project Settings.


#include "Tools/VillagePathManager.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Tools/PathManager.h"
#include "Components/SplineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/LightComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"


/*

TO DO:
finish at 167*

TryStartCutscene

*/

AVillagePathManager::AVillagePathManager()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AVillagePathManager::BeginPlay()
{
    Super::BeginPlay();

    if (!PathManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("A skeleton is missing a path"));
    }

    if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
    {
        if (ULocalPlayer* LP = PC->GetLocalPlayer())
        {
            if (UEnhancedInputLocalPlayerSubsystem* Subsys =
                LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
            {
                if (CutsceneIMC)
                {
                    Subsys->AddMappingContext(CutsceneIMC, 0);
                }
            }
        }

        if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PC->InputComponent))
        {
            if (StartCutsceneAction)
            {
                EIC->BindAction(StartCutsceneAction, ETriggerEvent::Started,
                    this, &AVillagePathManager::HandleStartCutsceneInput);
            }
        }
    }

    // debug
    /*
    bCutsceneStarted = true;
    StartVillageCutscene();
    */
}

USplineComponent* AVillagePathManager::GetSplineForSkeleton(AActor* Skeleton) const
{
    if (!Skeleton || !PathManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Missing Skeleton or PathManager."));
        return nullptr;
    }

    AActor* EndActor = nullptr;

    for (const FSkeletonVillageRoute& Route : SkeletonRoutes)
    {
        if (Route.Skeleton == Skeleton)
        {
            EndActor = Route.EndPoint;
            break;
        }
    }

    if (!EndActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: No route configured for skeleton %s"),
            *GetNameSafe(Skeleton));
        return nullptr;
    }

    const int32 StartIdx = PathManager->GetClosestStartIndex(Skeleton->GetActorLocation());
    if (StartIdx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: No valid StartIndex for skeleton %s"),
            *GetNameSafe(Skeleton));
        return nullptr;
    }

    int32 EndIdx = INDEX_NONE;
    for (int32 i = 0; i < PathManager->EndPoints.Num(); ++i)
    {
        if (PathManager->EndPoints[i] == EndActor)
        {
            EndIdx = i;
            break;
        }
    }

    if (EndIdx == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("VillagePathManager: EndPoint %s is not in PathManager->EndPoints"),
            *GetNameSafe(EndActor));
        return nullptr;
    }

    return PathManager->GetSplineForIndices(StartIdx, EndIdx);
}

void AVillagePathManager::OnSkeletonReachedEnd(AActor* Skeleton)
{
    if (!Skeleton)
    {
        return;
    }

    for (FSkeletonVillageRoute& Route : SkeletonRoutes)
    {
        if (Route.Skeleton == Skeleton && Route.HouseLight)
        {
            if (ULightComponent* LightComp =
                Route.HouseLight->FindComponentByClass<ULightComponent>())
            {
                LightComp->SetVisibility(true);
                LightComp->SetHiddenInGame(false);
            }
            else
            {
                Route.HouseLight->SetActorHiddenInGame(false);
            }

            Route.bReachedHome = true;

            break;
        }
    }

    bool bAllHome = true;

    for (const FSkeletonVillageRoute& R : SkeletonRoutes)
    {
        if (!R.Skeleton || !R.HouseLight) { continue; }

        if (!R.bReachedHome)
        {
            bAllHome = false;
            break;
        }
    }

    TryStartCutscene();

}

void AVillagePathManager::StartVillageCutscene_Implementation()
{
    if (!CutsceneSequenceActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("CutsceneSequenceActor is null."));
        return;
    }

    if (ULevelSequencePlayer* Player = CutsceneSequenceActor->GetSequencePlayer())
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            PC->SetCinematicMode(true, true, true, true, true);
        }

        Player->Play();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SequencePlayer is null on CutsceneSequenceActor."));
    }
}


void AVillagePathManager::TryStartCutscene()
{
    if (bCutsceneStarted)
    {
        return;
    }

    bool bAllHome = true;

    for (const FSkeletonVillageRoute& R : SkeletonRoutes)
    {
        if (!R.Skeleton || !R.HouseLight) { continue; }
        if (!R.bReachedHome) { bAllHome = false; break; }
    }

    if (bAllHome)
    {
        bCutsceneStarted = true;
        StartVillageCutscene();
    }
}

void AVillagePathManager::HandleStartCutsceneInput()
{
    if (!bCutsceneStarted)
    {
        bCutsceneStarted = true;
        StartVillageCutscene();
    }
}