#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VillageBorder.generated.h"

class UBoxComponent;
class UBillboardComponent;

UCLASS()
class PIPOUMANCIENTEAM2_API AVillageBorder : public AActor
{
    GENERATED_BODY()

public:
    AVillageBorder();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "VillageBorder")
    UBoxComponent* Trigger;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VillageBorder")
    FVector BoxExtent = FVector(200.f, 600.f, 200.f);

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
