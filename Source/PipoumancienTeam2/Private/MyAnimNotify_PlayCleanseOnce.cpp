// Fill out your copyright notice in the Description page of Project Settings.

#include "MyAnimNotify_PlayCleanseOnce.h"
#include "Components/SkeletalMeshComponent.h"
#include "PNJ/SkeletonController.h"
#include "TimerManager.h"

void UMyAnimNotify_PlayCleanseOnce::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (!MeshComp) return;

    AActor* Owner = MeshComp->GetOwner();
    if (!Owner) return;

    ASkeletonController* Skel = Cast<ASkeletonController>(Owner);
    if (!Skel) return;

    if (!Skel->bCleanseWindowActive) return;
    if (Skel->bCleanseFXPlayed)      return;

    Skel->bCleanseFXPlayed = true;

    TArray<USceneComponent*> Children;
    Owner->GetRootComponent()->GetChildrenComponents(true, Children);

    for (USceneComponent* Child : Children)
    {
        if (Child && Child->GetName().Contains(TEXT("Niagara"), ESearchCase::IgnoreCase))
        {
            UE_LOG(LogTemp, Warning, TEXT("%s: Found Niagara component %s"), *Owner->GetName(), *Child->GetName());

            Child->SetWorldScale3D(FVector(1.f, 1.f, 1.f));

            FTimerHandle TimerHandle;
            Owner->GetWorldTimerManager().SetTimer(
                TimerHandle,
                FTimerDelegate::CreateLambda([Child]()
                    {
                        if (IsValid(Child))
                        {
                            Child->SetWorldScale3D(FVector::ZeroVector);
                        }
                    }),
                3.0f,
                false
            );
        }
    }

    Super::Notify(MeshComp, Animation);
}
