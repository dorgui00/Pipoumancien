#include "Tools/VillageBorder.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Engine/Texture2D.h"

AVillageBorder::AVillageBorder()
{
    PrimaryActorTick.bCanEverTick = false;

    Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
    RootComponent = Trigger;

    Trigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    Trigger->SetCollisionResponseToAllChannels(ECR_Overlap);
    Trigger->SetGenerateOverlapEvents(true);
    Trigger->SetBoxExtent(BoxExtent, false);
    Trigger->SetHiddenInGame(true);

    Tags.AddUnique(FName("VillageBorder"));

#if WITH_EDITORONLY_DATA
    UBillboardComponent* Sprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
    Sprite->SetupAttachment(RootComponent);
    static ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTex(TEXT("/Engine/EditorResources/S_Trigger"));
    if (SpriteTex.Succeeded())
    {
        Sprite->SetSprite(SpriteTex.Object);
    }
    Sprite->bIsScreenSizeScaled = true;
    Sprite->SetHiddenInGame(true);
#endif
}

#if WITH_EDITOR
void AVillageBorder::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (Trigger)
    {
        Trigger->SetBoxExtent(BoxExtent, false);
    }
}
#endif