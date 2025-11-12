// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "PipoumancienTeam2/Public/Camera/CameraFollowTarget.h"


void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
	// get/set main camera
	CameraMain = FindCameraByTag(TEXT("CameraMain"));
	// camera look at rotation
	InitCameraRotationToPivot();
	// set init camera pos (main cam)
	InitMainCameraTransform = CameraMain->GetRelativeTransform();
	
	// get/set music camera 
	// World Camera ? Component Camera in BP_Camera ?
	
	//bounds
	// AActor* CameraBoundsActor = FindCameraBoundsActor();
	// if (CameraBoundsActor != nullptr)
	// {
	// 	InitCameraBounds(CameraBoundsActor);
	// }

	//zoom
	//InitCameraZoomParameters();
	
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//TickUpdateCameraZoom(DeltaTime);
	
	if (IsSettingCamera)
	{
		SetCamera(DeltaTime, IsWorldTransform);
		return;
	}
	
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


TArray<FVector> UCameraWorldSubsystem::GetCameraQuadGroundBounds()
{
    TArray<FVector> Points;
    if (!CameraMain || !GetWorld()) return Points;

    const FVector GroundNormal = FVector::UpVector;
    const float GroundZ = 0.f; // default

    // Get viewport size from player controller
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return Points;

    int32 ViewX = 0, ViewY = 0;
    PC->GetViewportSize(ViewX, ViewY);
    if (ViewX == 0 || ViewY == 0) return Points;

    // Screen corners 
    TArray<FVector2D> ScreenCorners = {
        FVector2D(0, 0),
        FVector2D(ViewX, 0),
        FVector2D(ViewX, ViewY),
        FVector2D(0, ViewY)
    };

	// from screen to ground world space
    for (const FVector2D& ScreenPos : ScreenCorners)
    {
        FVector WorldOrigin, WorldDir;
        PC->DeprojectScreenPositionToWorld(ScreenPos.X, ScreenPos.Y, WorldOrigin, WorldDir);

        float Denom = FVector::DotProduct(WorldDir, GroundNormal);
        if (FMath::Abs(Denom) > KINDA_SMALL_NUMBER)
        {
            float t = (GroundZ - WorldOrigin.Z) / Denom;
            FVector HitPoint = WorldOrigin + t * WorldDir;
            Points.Add(HitPoint);
        }
        else
        {
            // secu
            Points.Add(WorldOrigin + WorldDir * 100000.f);
        }
    }

	//debug quad
    // if (Points.Num() >= 4)
    // {
    //     for (int i = 0; i < 4; ++i)
    //     {
    //         DrawDebugLine(GetWorld(), Points[i], Points[(i + 1) % 4], FColor::Blue, false, 2.f, 0, 2.f);
    //     }
    // }

    return Points;
}

// return true if inside / return false if outside
bool UCameraWorldSubsystem::ClampPositionInsideQuad(const FVector& InPos, FVector& OutPos)
{
    TArray<FVector> Points = GetCameraQuadGroundBounds();

    if (Points.Num() < 3)
    {
        OutPos = InPos;
        return true; // default inside
    }

    // 3d to quad2D
    TArray<FVector2D> Quad;
    Quad.Reserve(Points.Num());
    for (const FVector& P3 : Points) Quad.Add(FVector2D(P3.X, P3.Y));

    FVector2D CameraPos2D(InPos.X, InPos.Y);

    // check if point (camera) is in quad
    auto PointInQuad = [](const FVector2D& Point, const TArray<FVector2D>& Quad) -> bool
    {
        bool Inside = false; 
        int32 Num = Quad.Num();
        for (int32 i = 0, j = Num - 1; i < Num; j = i++)
        {
            const FVector2D& Pi = Quad[i];
            const FVector2D& Pj = Quad[j];

        	//
            bool intersect = ((Pi.Y > Point.Y) != (Pj.Y > Point.Y)) && // point on IJ
                             (Point.X < (Pj.X - Pi.X) * (Point.Y - Pi.Y) / (Pj.Y - Pi.Y + KINDA_SMALL_NUMBER) + Pi.X); // if on IJ, check if point is on the left (p.x <) of intersect
            if (intersect)
                Inside = !Inside; // intersection is pair => outside / impair => inside
        }
        return Inside;
    };

    bool IsInside = PointInQuad(CameraPos2D, Quad);

    if (IsInside)
    {
        OutPos = InPos;
        return true; //inside
    }

    // if outside projection on closest point of quad
    auto ClosestPointOnSegment2D = [](const FVector2D& A, const FVector2D& B, const FVector2D& Point) -> FVector2D
    {
        FVector2D AB = B - A;
        float Den = FVector2D::DotProduct(AB, AB);
        if (Den <= KINDA_SMALL_NUMBER) return A;
        float t = FVector2D::DotProduct(Point - A, AB) / Den;
        t = FMath::Clamp(t, 0.f, 1.f);
        return A + AB * t;
    };

    float BestDistSqr = FLT_MAX;
    FVector2D BestClosest = FVector2D::ZeroVector;
    int32 NumEdges = Quad.Num();
    for (int i = 0; i < NumEdges; ++i)
    {
        const FVector2D& A = Quad[i];
        const FVector2D& B = Quad[(i + 1) % NumEdges];
        FVector2D Cand = ClosestPointOnSegment2D(A, B, CameraPos2D);
        float DistSqr = FVector2D::DistSquared(Cand, CameraPos2D);
        if (DistSqr < BestDistSqr)
        {
            BestDistSqr = DistSqr;
            BestClosest = Cand;
        }
    }

    OutPos = FVector(BestClosest.X, BestClosest.Y, InPos.Z);

    return false;
}

void UCameraWorldSubsystem::TickUpdateCameraZoom(float DeltaTime)
{
	if (CameraMain==nullptr) return;
	float GreatestDistanceBetweenTargets = CalculateGreatestDistanceBetweenTargets();
	
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
	
	//followtarget
	 if (CameraMain!=nullptr)
	 	CameraMain->GetOwner()->SetActorLocation(AveragePosition);
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


void UCameraWorldSubsystem::CallCamera(const ECameraType CameraType)
{
	switch (CameraType)
	{
		case ECameraType::GlobalCamera :
				AimedCameraTransform = InitMainCameraTransform ; // relative transform
				IsWorldTransform = false;
			
				break;
			
		case ECameraType::Dialogue:
				// Set pos of Dialogue Cam (middle of the speaker && player)
				//IsWorldTransform = ? 
				break;
			
		case ECameraType::MusicCamera:
			{
				UCameraComponent* MusicCamera = nullptr;

				//Find Music Camera in child components
				TArray<UActorComponent*> Components =  CameraMain->GetOwner()->GetComponentsByTag(USceneComponent::StaticClass(),FName("MusicCamera"));
				if (Components.Num() != 0)
				{
					MusicCamera = Cast<UCameraComponent>(Components[0]);

					AimedCameraTransform = MusicCamera->GetComponentTransform();
			
					IsWorldTransform = true;
				}
			
				break;
			}
		default :
			UE_LOG(LogTemp, Warning, TEXT("SwitchCamera is not a valid"));
			break;
	}

	IsSettingCamera = true;
}

// GET WORLD TRANSFORM 
void UCameraWorldSubsystem::SetCamera(float DeltaTime, bool IsWorld)
{
	if (IsWorld)
	{
		FVector NewPos = FMath::Lerp(CameraMain->GetComponentLocation(),AimedCameraTransform.GetLocation(),DeltaTime*1.f);
		FRotator NewRot = FMath::Lerp(CameraMain->GetComponentRotation(),AimedCameraTransform.Rotator(),DeltaTime*1.f);
		
		CameraMain->SetWorldLocationAndRotation(NewPos,NewRot);
		
		if (FMath::IsNearlyEqual(CameraMain->GetComponentLocation().Z,AimedCameraTransform.GetLocation().Z))
		{
			IsSettingCamera = false;
		}
	}
	else
	{
		
		FVector NewPos = FMath::Lerp(CameraMain->GetRelativeLocation(),AimedCameraTransform.GetLocation(),DeltaTime*1.f);
		FRotator NewRot = FMath::Lerp(CameraMain->GetRelativeRotation(),AimedCameraTransform.Rotator(),DeltaTime*1.f);

		CameraMain->SetRelativeLocationAndRotation(NewPos,NewRot);

		FVector g = CameraMain->GetRelativeLocation() - AimedCameraTransform.GetLocation();
		if (g.IsNearlyZero())
			IsSettingCamera = false;
	}


	
}

UCameraComponent* UCameraWorldSubsystem::FindCameraByTag(const FName& Tag) const
{
	//Retourne un composant UCameraComponent en recherchant un actor dans la scène à l’aide de son tag.

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag,Cameras);
	
	UCameraComponent* CameraComponent = nullptr;
	if (Cameras.Num() > 0)
	{
		CameraComponent = Cameras[0]->FindComponentByClass<UCameraComponent>();
	}
	
	return CameraComponent;
}

void UCameraWorldSubsystem::InitCameraRotationToPivot()
{
	if (CameraMain ==nullptr) return;

	AActor* CameraActor = CameraMain->GetOwner();
	if (CameraActor ==nullptr) return;

	FVector PivotPos = CameraActor->GetActorLocation();

	// cam component look at pivot
	FVector CamLocation = CameraMain->GetComponentLocation();
	FVector DirToPivot = (PivotPos - CamLocation).GetSafeNormal();
	FRotator LookAtRotation = DirToPivot.Rotation();

	CameraMain->SetWorldRotation(LookAtRotation);
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
	CameraBoundsMin = FVector2D(BoundsCenter.X-BoundsExtents.X,BoundsCenter.Y-BoundsExtents.Y);
	CameraBoundsMax = FVector2D(BoundsCenter.X+BoundsExtents.X,BoundsCenter.Y+BoundsExtents.Y);
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
