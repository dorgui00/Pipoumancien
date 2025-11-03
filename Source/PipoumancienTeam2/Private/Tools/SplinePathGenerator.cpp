#include "Tools/SplinePathGenerator.h"
#include "Components/SplineComponent.h"
#include "Components/BillboardComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "Engine/Texture2D.h"
#include "Engine/TargetPoint.h"

ASplinePathGenerator::ASplinePathGenerator()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

    StartPointClass = ATargetPoint::StaticClass();

#if WITH_EDITORONLY_DATA
    bIsEditorOnlyActor = true;
    UBillboardComponent* Sprite = CreateDefaultSubobject<UBillboardComponent>(TEXT("EditorIcon"));
    Sprite->SetupAttachment(RootComponent);
    Sprite->SetRelativeLocation(FVector::ZeroVector);
    Sprite->SetHiddenInGame(true);

    static ConstructorHelpers::FObjectFinder<UTexture2D> SpriteTexture(TEXT("/Engine/EditorResources/S_Note"));
    if (SpriteTexture.Succeeded())
    {
        Sprite->SetSprite(SpriteTexture.Object);
    }
#endif
}

#if WITH_EDITOR

static FColor ColorForStartIndex(int32 Index)
{
    const uint8 H = static_cast<uint8>((Index * 53) % 255);
    return FLinearColor::MakeFromHSV8(H, 200, 255).ToFColor(true);
}

void ASplinePathGenerator::ClearSplines()
{
    Modify(); //undo

    for (USplineComponent* Spline : GeneratedSplines)
    {
        if (Spline)
        {
            Spline->Modify(); //undo  
            Spline->DestroyComponent();
        }
    }
    GeneratedSplines.Empty();
}

void ASplinePathGenerator::RegenerateSplines()
{
    Modify();
    ClearSplines();

    StartPoints.RemoveAll([](AActor* A) { return A == nullptr; });
    EndPoints.RemoveAll([](AActor* A) { return A == nullptr; });

    if (StartPoints.Num() == 0 || EndPoints.Num() == 0)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    for (int32 StartIdx = 0; StartIdx < StartPoints.Num(); ++StartIdx)
    {
        AActor* Start = StartPoints[StartIdx];
        if (!Start) continue;

        const FColor StartColor = ColorForStartIndex(StartIdx);

        for (AActor* End : EndPoints)
        {
            if (!End) continue;

            USplineComponent* NewSpline =
                NewObject<USplineComponent>(this, USplineComponent::StaticClass(), NAME_None, RF_Transactional);

            NewSpline->SetupAttachment(RootComponent);
            NewSpline->RegisterComponent();
            NewSpline->SetMobility(EComponentMobility::Movable);
            NewSpline->Modify();

            NewSpline->ClearSplinePoints(false);
            NewSpline->AddSplinePoint(Start->GetActorLocation(), ESplineCoordinateSpace::World, false);
            NewSpline->AddSplinePoint(End->GetActorLocation(), ESplineCoordinateSpace::World, true);
            NewSpline->SetClosedLoop(false);
            NewSpline->SetSplinePointType(0, ESplinePointType::Curve, false);
            NewSpline->SetSplinePointType(1, ESplinePointType::Curve, true);

            DrawDebugLine(World, Start->GetActorLocation(), End->GetActorLocation(), StartColor, false, 100.f, 0, 3.f);

            GeneratedSplines.Add(NewSpline);
        }
    }
}

void ASplinePathGenerator::AddEndpoint()
{
    Modify();

    StartPoints.RemoveAll([](AActor* A) { return A == nullptr; });
    if (StartPoints.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("SplinePathGenerator: No StartPoints set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    const AActor* RefStart = nullptr;
    for (int32 i = StartPoints.Num() - 1; i >= 0; --i)
    {
        if (StartPoints[i]) { RefStart = StartPoints[i]; break; }
    }

    const FVector BaseLoc = RefStart ? RefStart->GetActorLocation() : GetActorLocation();
    const FVector SpawnLoc = BaseLoc + FVector(200.f * (EndPoints.Num() + 1), 0.f, 0.f);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewEndPoint = World->SpawnActor<AActor>(
        ATargetPoint::StaticClass(), SpawnLoc, FRotator::ZeroRotator, Params);

    if (NewEndPoint)
    {
        NewEndPoint->SetActorLabel(FString::Printf(TEXT("EndPoint_%02d"), EndPoints.Num()));
        EndPoints.Add(NewEndPoint);
    }

    RegenerateSplines();
}

void ASplinePathGenerator::AddStartPoint()
{
    Modify();

    UWorld* World = GetWorld();
    if (!World) return;

    UClass* ClassToSpawn = StartPointClass ? *StartPointClass : ATargetPoint::StaticClass();

    const FVector SpawnLoc = GetActorLocation() + FVector(-200.f * (StartPoints.Num() + 1), 0.f, 0.f);

    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    AActor* NewStart = World->SpawnActor<AActor>(ClassToSpawn, SpawnLoc, FRotator::ZeroRotator, Params);

    if (NewStart)
    {
        NewStart->SetActorLabel(FString::Printf(TEXT("StartPoint_%02d"), StartPoints.Num()));
        StartPoints.Add(NewStart);
    }

    RegenerateSplines();
}

void ASplinePathGenerator::UpdateDebugVisuals()
{
    if (GeneratedSplines.Num() == 0)
    {
        return;
    }

    UWorld* World = GetWorld();
    if (!World)
        return;

    for (USplineComponent* Spline : GeneratedSplines)
    {
        if (!Spline) continue;

        const int32 NumPoints = Spline->GetNumberOfSplinePoints();
        if (NumPoints < 2)
            continue;

        for (int32 i = 0; i < NumPoints - 1; ++i)
        {
            const FVector Start = Spline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
            const FVector End = Spline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

            DrawDebugLine(
                World,
                Start,
                End,
                FLinearColor::MakeRandomColor().ToFColor(true),
                false,
                10.f,
                0,
                3.0f
            );
        }
    }
}
#endif