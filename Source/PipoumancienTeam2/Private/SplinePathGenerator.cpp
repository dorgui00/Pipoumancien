#include "SplinePathGenerator.h"
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


void ASplinePathGenerator::ClearSplines()
{
    Modify(); //undo

    for (USplineComponent* Spline : GeneratedSplines)
    {
        if (Spline)
        {
            Spline->Modify(); //deletion    
            Spline->DestroyComponent();
        }
    }
    GeneratedSplines.Empty();
}

void ASplinePathGenerator::RegenerateSplines()
{
    Modify(); //undo
    ClearSplines();

    if (!StartPoint)
    {
        return;
    }

    for (AActor* EndPoint : EndPoints)
    {
        if (!EndPoint) continue;

        USplineComponent* NewSpline =
            NewObject<USplineComponent>(this, USplineComponent::StaticClass(), NAME_None, RF_Transactional);

        NewSpline->RegisterComponent(); //register in world/hierarchy


        NewSpline->SetMobility(EComponentMobility::Movable);
        NewSpline->Modify(); //undo

        NewSpline->ClearSplinePoints(false);
        NewSpline->AddSplinePoint(StartPoint->GetActorLocation(), ESplineCoordinateSpace::World, false);
        NewSpline->AddSplinePoint(EndPoint->GetActorLocation(), ESplineCoordinateSpace::World, true);
        NewSpline->SetClosedLoop(false);
        NewSpline->SetSplinePointType(0, ESplinePointType::Curve, false);
        NewSpline->SetSplinePointType(1, ESplinePointType::Curve, true);

        DrawDebugLine(
            GetWorld(),
            StartPoint->GetActorLocation(),
            EndPoint->GetActorLocation(),
            FLinearColor::MakeRandomColor().ToFColor(true),
            false, 10.f, 0, 3.f
        );

        GeneratedSplines.Add(NewSpline);
    }
}

void ASplinePathGenerator::AddEndpoint()
{
    Modify(); //undo

    if (!StartPoint)
    {
        UE_LOG(LogTemp, Warning, TEXT("SplinePathGenerator: No StartPoint set!"));
        return;
    }

    UWorld* World = GetWorld();
    if (!World) return;

    AActor* NewEndPoint = World->SpawnActor<AActor>(
        ATargetPoint::StaticClass(),
        StartPoint->GetActorLocation() + FVector(200, 0, 0),
        FRotator::ZeroRotator
    );

    if (NewEndPoint)
    {
        NewEndPoint->SetActorLabel(TEXT("New EndPoint"));
        EndPoints.Add(NewEndPoint);
    }

    RegenerateSplines();
}

void ASplinePathGenerator::AddStartPoint()
{
    Modify(); //undo

    UWorld* World = GetWorld();
    if (!World)
        return;

    AActor* NewStart = World->SpawnActor<AActor>(
        ATargetPoint::StaticClass(),
        GetActorLocation() + FVector(-200, 0, 0),
        FRotator::ZeroRotator
    );

    if (NewStart)
    {
        NewStart->SetActorLabel(TEXT("StartPoint"));
        StartPoint = NewStart;
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
