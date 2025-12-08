// AnimNotify_WalkFollowFX.cpp

#include "AnimNotify_WalkFollowFX.h"
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateWalk.h"

void UAnimNotify_WalkFollowFX::Notify(
    USkeletalMeshComponent* MeshComp,
    UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    if (!MeshComp)
        return;

    APipouCharacter* Pipou =
        Cast<APipouCharacter>(MeshComp->GetOwner());
    if (!Pipou)
        return;

    UPipouCharacterStateWalk* WalkState =
        Pipou->FindComponentByClass<UPipouCharacterStateWalk>();
    if (!WalkState || !WalkState->bFollowEffectActive)
    {
        return;
    }
    else 
    {
        Super::Notify(MeshComp, Animation, EventReference);
    }
}
