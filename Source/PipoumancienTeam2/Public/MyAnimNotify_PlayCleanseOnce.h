// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_PlayNiagaraEffect.h"
#include "MyAnimNotify_PlayCleanseOnce.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API UMyAnimNotify_PlayCleanseOnce : public UAnimNotify_PlayNiagaraEffect
{
    GENERATED_BODY()

public:
    virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
};
