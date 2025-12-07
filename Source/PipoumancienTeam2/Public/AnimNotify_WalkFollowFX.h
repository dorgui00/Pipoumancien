// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "AnimNotify_WalkFollowFX.generated.h"

/**
 * 
 */

UCLASS()
class PIPOUMANCIENTEAM2_API UAnimNotify_WalkFollowFX
    : public UAnimNotify_PlayNiagaraEffect
{
    GENERATED_BODY()

public:
    virtual void Notify(
        USkeletalMeshComponent* MeshComp,
        UAnimSequenceBase* Animation,
        const FAnimNotifyEventReference& EventReference
    ) override;
};

