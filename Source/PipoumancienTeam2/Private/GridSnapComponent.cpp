
#include "GridSnapComponent.h"
#include "GridGenerator.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#endif

UGridSnapComponent::UGridSnapComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
void UGridSnapComponent::OnRegister()
{
    Super::OnRegister();
    if (GIsEditor && GEditor && GetWorld() && !GetWorld()->IsGameWorld())
    {
        GEditor->OnEndObjectMovement().AddUObject(this, &UGridSnapComponent::HandleEndObjectMovement);
    }
}

void UGridSnapComponent::OnUnregister()
{
    if (GIsEditor && GEditor)
    {
        GEditor->OnEndObjectMovement().RemoveAll(this);
    }
    Super::OnUnregister();
}

void UGridSnapComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    SnapOwner();
}

void UGridSnapComponent::PostEditUndo()
{
    Super::PostEditUndo();
    SnapOwner();
}

void UGridSnapComponent::HandleEndObjectMovement(UObject& Object)
{
    if (AActor* Owner = GetOwner())
    {
        if (&Object == Owner)
        {
            SnapOwner();
        }
    }
}

void UGridSnapComponent::SnapOwner()
{
    AActor* Owner = GetOwner();
    if (!Owner) return;

    const float Size = ResolveGridSize();
    const FVector Loc = Owner->GetActorLocation();
    const FVector Snapped = Snap(Loc, Size);

    if (!Snapped.Equals(Loc, 0.01f))
    {
        Owner->Modify();
        Owner->SetActorLocation(Snapped, false, nullptr, ETeleportType::TeleportPhysics);
    }
}

FVector UGridSnapComponent::Snap(const FVector& In, float Size) const
{
    FVector Out = In;
    Out.X = FMath::GridSnap(Out.X, Size);
    Out.Y = FMath::GridSnap(Out.Y, Size);
    if (bSnapZ)
    {
        Out.Z = FMath::GridSnap(Out.Z, Size);
    }
    return Out;
}

float UGridSnapComponent::ResolveGridSize() const
{
    if (bOverrideGridSize) return FMath::Max(1.f, GridSizeOverride);
    if (CachedGrid.IsValid()) return CachedGrid.Get()->GridSize;

    float BestDistSq = TNumericLimits<float>::Max();
    AGridGenerator* Best = nullptr;

    if (UWorld* World = GetWorld())
    {
        const FVector Here = GetOwner() ? GetOwner()->GetActorLocation() : FVector::ZeroVector;
        for (TActorIterator<AGridGenerator> It(World); It; ++It)
        {
            const float D2 = FVector::DistSquared(It->GetActorLocation(), Here);
            if (D2 < BestDistSq) { BestDistSq = D2; Best = *It; }
        }
    }

    if (Best) { CachedGrid = Best; return Best->GridSize; }
    return 100.f;
}
#endif
