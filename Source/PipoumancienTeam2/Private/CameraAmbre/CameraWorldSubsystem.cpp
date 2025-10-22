// Fill out your copyright notice in the Description page of Project Settings.

#include "PipoumancienTeam2/Public/CameraAmbre/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"

#include "Elements/Columns/TypedElementCompatibilityColumns.h"
#include "Kismet/GameplayStatics.h"
#include "PipoumancienTeam2/Public/CameraAmbre/CameraFollowTarget.h"


void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	CameraMain = FindCameraByTag(TEXT("CameraMain"));

	AActor* CameraBoundsActor = FindCameraBoundsActor();
	if (CameraBoundsActor != nullptr)
	{
		InitCameraBounds(CameraBoundsActor);
	}

	//InitCameraZoomParameters();
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraPosition(DeltaTime);
}

void UCameraWorldSubsystem::AddFollowTarget(UObject* FollowTarget)
{
	//Ajoute une cible à suivre dans le tableau FollowTargets.
	FollowTargets.Add(FollowTarget);
}

void UCameraWorldSubsystem::RemoveFollowTarget(UObject* FollowTarget)
{
	//Supprime une cible à suivre dans le tableau FollowTargets.
	FollowTargets.Remove(FollowTarget);
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (CameraMain==nullptr) return;
	float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();

	//find current % of distance using
	// - GreatestDistanceBetweenTargets
	// - CameraZoomDistanceBetwweenTargetsMin
	// - CameraZoomDistanceBetwweenTargetsMax
	//inverse lerp unreal has find name
	//clamp percent btw 0& 1
	
	float percent = FMath::GetRangePct(CameraZoomDistanceBetweenTargetsMin,CameraZoomDistanceBetweenTargetsMax,GreatestDistanceBetweenTargets);
	percent = FMath::Clamp(percent, 0.0f, 1.0f);

	//Update main camera Y pos
	float posY= FMath::Lerp(CameraZoomYMin,CameraZoomYMax,percent);
	
	FVector pos = FVector(CameraMain->GetOwner()->GetActorLocation().X,posY,CameraMain->GetOwner()->GetActorLocation().Z);
	CameraMain->SetWorldLocation(pos);
}

void UCameraWorldSubsystem::TickUpdateCameraPosition(float DeltaTime)
{
	FVector AveragePosition = CalculateAveragePositionBetweenTargets();
	
	//Met à jour la position de la caméra (en fonction des cibles à suivre).
	//CameraMain->SetWorldLocation(FVector(AveragePosition.X,AveragePosition.Y,CameraMain->GetComponentLocation().Z));
	if (CameraMain!=nullptr)
		CameraMain->GetOwner()->SetActorLocation(AveragePosition);
	//TranslateFollowTargets();
}

FVector UCameraWorldSubsystem::CalculateAveragePositionBetweenTargets()
{
	//Calcule la moyenne de positions entre toutes les cibles du tableau FollowTargets.
	FVector Tot = FVector::ZeroVector;

	for (auto Target : FollowTargets)
	{
		//Uniquement si l’objet dans la liste implémente l’interface 
		if (Target !=nullptr && Target->Implements<UCameraFollowTarget>())
		{
			if (ICameraFollowTarget* FollowTarget = Cast<ICameraFollowTarget>(Target))
			{
				if (FollowTarget->IsFollowable())
				{
					Tot += FollowTarget->GetFollowPosition();
				}
			}
		};
	}
	FVector Moy = Tot/FollowTargets.Num();
	
	return Moy;
}

float UCameraWorldSubsystem::CalculateGreatestDistanceBetweenTargets()
{
	// new array of valid pos
	TArray<FVector> Positions;
	
	for (auto* Target : FollowTargets)
	{
		if (auto* IInterface = Cast<ICameraFollowTarget>(Target))
		{
			Positions.Add(IInterface->GetFollowPosition());
		}
	}
	
	//calculate greatest distance
	float GreatestDistance = 0.0f;
	
	for (int i = 0; i < Positions.Num(); ++i)
	{
		for (int j=i+1; j<Positions.Num(); ++j)
		{			
			float NewDistance = (Positions[i]-Positions[j]).Size();

			if (NewDistance > GreatestDistance)
				GreatestDistance = NewDistance;
		}
	}
			
	return GreatestDistance;
}

void UCameraWorldSubsystem::TranslateFollowTargets()
{
	if (!CameraMain || !GetWorld()) return;

    // 
    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_GameTraceChannel2); 

    // 
    FVector CameraPos = CameraMain->GetComponentLocation();
    FVector LineEnd = CameraPos + CameraMain->GetForwardVector() * 1000.f;

    // 
    FHitResult ForwardHit;
    DrawDebugLine(GetWorld(), CameraPos, LineEnd, FColor::Green, false, 1.f, 0, 1.f);
    if (GetWorld()->LineTraceSingleByObjectType(ForwardHit, CameraPos, LineEnd, ObjectQueryParams))
    {
        DrawDebugSphere(GetWorld(), ForwardHit.ImpactPoint, 50.f, 20, FColor::Red, false, 1.f);
        UE_LOG(LogTemp, Warning, TEXT("Hit forward at %s (actor %s)"),
               *ForwardHit.ImpactPoint.ToString(),
               ForwardHit.GetActor() ? *ForwardHit.GetActor()->GetName() : TEXT("None"));
    }

    // vecteur directionnel
    FVector TraceStart = CameraPos;
    FVector TraceEnd = CameraPos - FVector(0.f, 0.f, 3000.f); // vers le bas
    FHitResult GroundHit;

    if (GetWorld()->LineTraceSingleByObjectType(GroundHit, TraceStart, TraceEnd, ObjectQueryParams))
    {
        FVector GroundPoint = GroundHit.ImpactPoint;

        // Debug
        DrawDebugSphere(GetWorld(), GroundPoint, 50.f, 20, FColor::Purple, false, 3.f);

        // 
        FVector CenterOfPlayers = CalculateAveragePositionBetweenTargets();
        DrawDebugSphere(GetWorld(), CenterOfPlayers, 50.f, 20, FColor::Yellow, false, 3.f);

        //
        FVector CameraXY = CameraPos;
        FVector CenterXY = FVector(CenterOfPlayers.X, CenterOfPlayers.Y, CameraXY.Z);

        FVector DirectionXY = CameraXY - CenterXY;
        DirectionXY.Z = 0.f; // lock Z

        // debug
        DrawDebugLine(GetWorld(), CenterXY, CenterXY + DirectionXY, FColor::Orange, false, 3.f, 0, 1.f);
    	
        FVector NewCameraPos = CenterXY + DirectionXY;

        CameraMain->SetWorldLocation(NewCameraPos);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No ground hit for orthogonal projection"));
    }
}

void UCameraWorldSubsystem::InitCameraZoomParameters()
{
	//Find CameraDistanceMin (using tag) and Update CameraZoomYMin according to Y position if found
	AActor* CameraDistanceMin = FindCameraByTag(TEXT("CameraDistanceMin"))->GetOwner();
	
	if (CameraDistanceMin != nullptr)
		CameraZoomYMin = CameraDistanceMin->GetActorLocation().Y;

	//Find CameraDistanceMax (using tag) and Update CameraZoomYMax according to Y position if found
	AActor* CameraDistanceMax = FindCameraByTag(TEXT("CameraDistanceMax"))->GetOwner();
	
	if (CameraDistanceMax != nullptr)
		CameraZoomYMax = CameraDistanceMax->GetActorLocation().Y;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	//Retourne un composant UCameraComponent en recherchant un actor dans la scène à l’aide de son tag.

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag,Cameras);
	
	UCameraComponent* Camera = nullptr;
	if (Cameras.Num() > 0)
	{
		Camera = Cameras[0]->FindComponentByClass<UCameraComponent>();
	}
	
	return Camera;
}

AActor* UCameraWorldSubsystem::FindCameraBoundsActor()
{
	TArray<AActor*> CameraBoundsActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), "CameraBounds",CameraBoundsActors);
	
	AActor* FirstCameraBoundsActor = nullptr;
	if (CameraBoundsActors.Num() > 0)
	{
		FirstCameraBoundsActor = CameraBoundsActors[0];
	}
	
	return FirstCameraBoundsActor;
}

void UCameraWorldSubsystem::InitCameraBounds(AActor* CameraBoundsActor)
{
	FVector BoundsCenter;
	FVector BoundsExtents;

	CameraBoundsActor->GetActorBounds(false,BoundsCenter,BoundsExtents);

	//Fill CameraBounds and CameraYProjectionCenter according to bounds
	CameraBoundsMin = FVector2D(BoundsCenter.X-BoundsExtents.X/2,BoundsCenter.Y-BoundsExtents.Y/2);
	CameraBoundsMax = FVector2D(BoundsCenter.X+BoundsExtents.X/2,BoundsCenter.Y+BoundsExtents.Y/2);
	CameraBoundsYProjectionCenter = BoundsCenter.Y; 
}

void UCameraWorldSubsystem::ClampPositionIntoCameraBounds(FVector& Position)
{
	FVector2D ViewportBoundsMin,ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin,ViewportBoundsMax);

	FVector WorldBoundsMin = CalculateWorldPositionFromViewportPosition(ViewportBoundsMin);
	FVector WorldBoundsMax = CalculateWorldPositionFromViewportPosition(ViewportBoundsMax);

	// Clamp Position according to  WorldBounds
	Position = ClampVector(Position, WorldBoundsMin,WorldBoundsMax );
}

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax)
{
	// Find Viewport
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	if (ViewportClient == nullptr) return;

	FViewport* Viewport = ViewportClient->Viewport;
	if (Viewport == nullptr) return;

	//Calculate viewport rect according to camera aspect ratio and viewport viewrect
	FIntRect ViewRect(
		Viewport->GetInitialPositionXY(),
		Viewport->GetInitialPositionXY()+ Viewport->GetSizeXY()
	);

	FIntRect ViewportRect = Viewport->CalculateViewExtents(CameraMain->AspectRatio, ViewRect);

	//Fill output parameters with ViewportRect
	OutViewportBoundsMin.X = ViewportRect.Min.X;
	OutViewportBoundsMin.Y = ViewportRect.Min.Y;

	OutViewportBoundsMax.X = ViewportRect.Max.X;
	OutViewportBoundsMax.Y = ViewportRect.Max.Y;
}

FVector UCameraWorldSubsystem::CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition)
{
	if (CameraMain == nullptr) return FVector::Zero();

	APlayerController* PlayerController= UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (PlayerController==nullptr) return FVector::Zero();

	float YDistanceToCenter = CameraMain->GetOwner()->GetActorLocation().Y - CameraBoundsYProjectionCenter;
	
	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		ViewportPosition,
		WorldPosition,
		CameraWorldProjectDir
		);
	
	WorldPosition += CameraWorldProjectDir * YDistanceToCenter;
	
	return WorldPosition;
}
