// Fill out your copyright notice in the Description page of Project Settings.

#include "Camera/CameraWorldSubsystem.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraVisibleTarget.h"
#include "Camera/FInvisibleObject.h"
#include "Character/PipouCharacterStateID.h"
#include "Character/PipouCharacterStateMachine.h"
#include "Concepts/Iterable.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PipoumancienTeam2/Public/Camera/CameraFollowTarget.h"
#include "PNJ/SkeletonController.h"
#include "Settings/SubsystemSettings.h"
#include "Enums/CollisionChannel.h"


void UCameraWorldSubsystem::PostInitialize()
{
	Super::PostInitialize();
}

void UCameraWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
	
}



void UCameraWorldSubsystem::InitCameraSubsystem()
{
	AssignAllCameras();
	
	InitMainCamera();
	
	InitCameraVisibility();

}



void UCameraWorldSubsystem::AssignAllCameras()
{
	// get/set main camera
	AActor* CameraActor = FindCameraActorByTag(TEXT("CameraMain"));
	
	
	CameraMain = FindCameraComponentByTag(CameraActor, ("CameraMain"));
	if(!CameraMain)
	{
		UE_LOG(LogTemp, Error, TEXT("Main Camera is null"));
		
	}
	
	// Init Dialogue Camera
	DialogueCamera = FindCameraComponentByTag(CameraActor, ("DialogueCamera"));
	if(!DialogueCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Dialogue Camera is null"));
	}
	
	// Init Music Camera
	MusicCamera = FindCameraComponentByTag(CameraActor, ("MusicCamera"));
	if(!MusicCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Music Camera is null"));
	}
	
	// Init Global Camera
	GlobalCamera = FindCameraComponentByTag(CameraActor, ("GlobalCamera"));
	if(!GlobalCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("Global Camera is null"));
	}
}


void UCameraWorldSubsystem::InitMainCamera()
{
	// // set init camera pos (main cam)
	if (GlobalCamera)
	CameraMain->SetRelativeLocation(GlobalCamera->GetRelativeLocation());
	
	// camera look at rotation
	InitCameraRotationToPivot();

	// is global camera
	CameraState = ECameraState::GlobalCamera;
}

void UCameraWorldSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// --- CINE ---
	if (ShouldSkipCameraForThisMap())
	{
		return;
	}
	
	// --- TICK ---
	
	//TickUpdateCameraZoom(DeltaTime);
	TickUpdateCameraVisibility(DeltaTime);
	
	// --- SETTING + ZOOM ---
	if (IsSettingCamera)
	{
		LerpCamera(DeltaTime);
		
		if (!IsZooming) return; // Zoom => continue default behaviour of current camera
	}

	//  --- CAMERAS TICK BEHAVIOUR ---
	if (CameraState == ECameraState::GlobalCamera)
	{
		TickUpdateCameraPosition(DeltaTime);
		
	}
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

void UCameraWorldSubsystem::Zoom(float Value)
{
	ResetLerpTimer(); // timer = 0
	
	IsZoomed = true;

	// default actor pos / rot
	CanLerpActor = false;

	// Component Pos / Rot
	CanLerpComponent = true;
	
	StartComponentTransform = CameraMain->GetRelativeTransform();
	
	EndComponentTransform = FTransform(StartComponentTransform);
	FVector EndPos = CameraMain->GetRelativeTransform().GetLocation() + CameraMain->GetForwardVector() * Value;
	EndComponentTransform.SetLocation(EndPos);

	// start Lerping
	IsSettingCamera = true;
}

void UCameraWorldSubsystem::Dezoom(float Value)
{
	ResetLerpTimer(); // timer = 0
	
	IsZoomed = false;

	// default actor pos / rot
	CanLerpActor = false;

	// Component Pos / Rot
	CanLerpComponent = true;

	StartComponentTransform = CameraMain->GetRelativeTransform();
	
	EndComponentTransform = FTransform(StartComponentTransform);
	FVector EndPos = CameraMain->GetRelativeTransform().GetLocation() - CameraMain->GetForwardVector() * Value;
	EndComponentTransform.SetLocation(EndPos);

	// start Lerping
	IsSettingCamera = true;
}

bool UCameraWorldSubsystem::GetIsZoomed()
{
	return IsZoomed;
}

void UCameraWorldSubsystem::SkeletonInteractionZoom(bool Zoom)
{
	ResetLerpTimer(); // timer = 0

	// --- ACTOR POS / ROT ---
	CanLerpActor = false;

	// --- COMPONENT POS / ROT ---
	CanLerpComponent = true;
	
	StartComponentTransform = CameraMain->GetRelativeTransform();

	// End Component
	EndComponentTransform = FTransform(StartComponentTransform);
	
	FVector EndPos;
	if (Zoom)
	{
		EndPos = GlobalCamera->GetRelativeTransform().GetLocation() + CameraMain->GetForwardVector() * SkeletonZoom;
		IsZooming = true;
	}
	else
	{
		EndPos = GlobalCamera->GetRelativeTransform().GetLocation();
		IsZooming = false;
		IsZooming = true;
	}
	
	EndComponentTransform.SetLocation(EndPos);

	// --- START LERPING --
	IsSettingCamera = true;
}


void UCameraWorldSubsystem::AddVisibleTarget(UObject* VisibleTarget)
{
	VisibleTargets.Add(VisibleTarget);
	
	SetVisibleTarget(VisibleTarget);
}

void UCameraWorldSubsystem::RemoveVisibleTarget(UObject* VisibleTarget)
{
	VisibleTargets.Remove(VisibleTarget);
}


void UCameraWorldSubsystem::SetVisibleTarget(UObject* VisibleTarget)
{
// 	if (AActor* test = Cast<AActor>(VisibleTarget))
// 	{
// 		test->GetComponentsCollisionResponseToChannel(COLLISION_CLOAK)
// 	}
}

void UCameraWorldSubsystem::InitCameraVisibility()
{
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	InvisibleMaterial =  SubsystemSettings->InvisibleMaterial.LoadSynchronous() ;
	
}


void UCameraWorldSubsystem::TickUpdateCameraVisibility(float DeltaTime)
{
	// TO EDIT => CLEANING
	// Viewport center
	FVector2D ViewportBoundsMin, ViewportBoundsMax;
	GetViewportBounds(ViewportBoundsMin,ViewportBoundsMax);

	float ViewportCenterX = (ViewportBoundsMin.X + ViewportBoundsMax.X)/2;
	float ViewportCenterY = (ViewportBoundsMin.Y + ViewportBoundsMax.Y)/2;
	FVector2D ViewportCenter = FVector2D(ViewportCenterX,ViewportCenterY);
	//FVector ViewportCenterToWorld = CalculateWorldPositionFromViewportPosition(ViewportCenter);

	APlayerController* PlayerController= UGameplayStatics::GetPlayerController(GetWorld(),0);
	if (PlayerController==nullptr) return;
	
	FVector CameraWorldProjectDir;
	FVector WorldPosition;
	UGameplayStatics::DeprojectScreenToWorld(
		PlayerController,
		ViewportCenter,
		WorldPosition,
		CameraWorldProjectDir
		);
	
	
	// foreach target multiple line trace
	FVector Pos = WorldPosition + (CameraMain->GetForwardVector() * 50.f);

	// --- DEBUG ---
	//DrawDebugLine(GetWorld(), WorldPosition, Pos, FColor::Blue, false, 2.f, 0, 2.f);
	

	// Reset
	TArray<struct FHitResult> OutHits;
	struct FHitResult OutHit;
	CurrentBlockingTest.Empty();
	
	//GetWorld()->LineTraceByChannel(OutHits,WorldPosition,Pos, COLLISION_CLOAK);
	GetWorld()->LineTraceSingleByChannel(OutHit,WorldPosition,Pos, COLLISION_CLOAK);
	
	if (OutHit.GetActor())
	{
		SetCloakingObjectBehaviour(OutHit);
	}
	// for (const auto& Hit : OutHits)
	// {
	// 	SetCloakingObjectBehaviour(Hit);
	// }
	
	// check if previous cloaking objet are no more cloaking
	CompareCurrentFromPreviousInvisibleObjects();
	
}

void UCameraWorldSubsystem::SetCloakingObjectBehaviour(const FHitResult& Hit)
{
	AActor* HitActor = Hit.GetActor();
	
	// DEBUG 
	//UE_LOG(LogTemp,Display,TEXT("Hit %s dvisible target "), *Hit.GetActor()->GetName());
			
	// Add to currently cloaking object list
	CurrentBlockingTest.Add(HitActor);
	
	if (!PreviousInvisibleTest.Contains(HitActor))
	{
		// Set invisibility
		HitActor->SetActorHiddenInGame(true);
		
		// update invisible object list
		PreviousInvisibleTest.AddUnique(HitActor);
	}
	
}


void UCameraWorldSubsystem::CompareCurrentFromPreviousInvisibleObjects()
{
	for (int i = 0; i < PreviousInvisibleTest.Num() ; i++)
	{
		// no longer invisible
		if (!CurrentBlockingTest.Contains(PreviousInvisibleTest[i]))
		{
			// remove from list
			MakeObjectVisibleAgain(PreviousInvisibleTest[i]);
		}
	}
	
}


void UCameraWorldSubsystem::MakeObjectVisibleAgain(TObjectPtr<AActor> InvisibleObject)
{
	//DEBUG 
	//UE_LOG(LogTemp, Display, TEXT("Plus invisible"));

	// Visible
	InvisibleObject->SetActorHiddenInGame(false);

	// Update list
	PreviousInvisibleTest.Remove(InvisibleObject);
	
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
	AActor* CameraDistanceMin = FindCameraActorByTag(TEXT("CameraDistanceMin"))->GetOwner();
	
	if (CameraDistanceMin != nullptr)
		CameraZoomYMin = CameraDistanceMin->GetActorLocation().Y;

	//Find CameraDistanceMax (using tag) and Update CameraZoomYMax according to Y position if found
	AActor* CameraDistanceMax = FindCameraActorByTag(TEXT("CameraDistanceMax"))->GetOwner();
	
	if (CameraDistanceMax != nullptr)
		CameraZoomYMax = CameraDistanceMax->GetActorLocation().Y;
}


ECameraState UCameraWorldSubsystem::GetState() const
{
	return CameraState;
}

void UCameraWorldSubsystem::SetMusicCamera(ASkeletonController* Skeleton)
{
	//transition
	ResetLerp();
	
	// Actor Pos 
	CanLerpActor = true;
	StartActorTransform = CameraMain->GetOwner()->GetActorTransform();
	EndActorTransform = CameraMain->GetOwner()->GetActorTransform();
	EndActorTransform.SetLocation(Skeleton->GetActorLocation());
	
	// Component Pos / Rot
	CanLerpComponent = true;
	StartComponentTransform = CameraMain->GetRelativeTransform();
	EndComponentTransform = MusicCamera->GetRelativeTransform();
		
	//Update State
	PreviousState = CameraState;
	NextState = ECameraState::MusicCamera;
	
	IsSettingCamera = true;
}

void UCameraWorldSubsystem::SetGlobalCamera()
{	
	//transition
	ResetLerp();
	
	//Camera Actor pos
	CanLerpActor = true;
	StartActorTransform = CameraMain->GetOwner()->GetActorTransform();
	FVector EndLocation = CalculateAveragePositionBetweenTargets();
	EndActorTransform.SetLocation(EndLocation);
	
	//Camera Component pos
	CanLerpComponent = true;
	StartComponentTransform = CameraMain->GetRelativeTransform();
	EndComponentTransform = GlobalCamera->GetRelativeTransform();

	// Main camera looks at the middle of the bp
	// target - look at
	FVector Forward =  - EndComponentTransform.GetLocation(); //CameraMain->GetRelativeLocation() - EndComponentTransform.GetLocation();
	FRotator Rot = UKismetMathLibrary::MakeRotFromXZ(Forward, FVector::UpVector);
	EndComponentTransform.SetRotation(Rot.Quaternion());

	//Update State
	PreviousState = CameraState;
	NextState = ECameraState::GlobalCamera;
	
	IsSettingCamera = true;
}


// camera moves closer, places itself between the two interlocutors but targets the speaker
void UCameraWorldSubsystem::SetDialogueCamera(const APipouCharacter* Interactor, ASkeletonController* Speaker)
{
	//transition
	ResetLerp();
	
	// Speaker look at interactor
	// forward = target - look at
	FVector Forward1 = Interactor->GetActorLocation() - Speaker->GetActorLocation();
	FRotator Rot1 = UKismetMathLibrary::MakeRotFromXZ(Forward1, FVector::UpVector);
	FRotator Rot2 = FRotator(Speaker->GetActorRotation().Pitch, Rot1.Yaw, Speaker->GetActorRotation().Roll);	
	Speaker->SetActorRotation(Rot2.Quaternion());
	
	// EndActorPos = BP Camera in the middle
	CanLerpActor = true;
	StartActorTransform = CameraMain->GetOwner()->GetActorTransform();
	FVector EndActorPosition =  (Interactor->GetActorLocation()+Speaker->GetActorLocation())*0.5f; // places itself in the middle
	EndActorTransform.SetLocation(EndActorPosition);

	
	// Dialogue Camera Component
	CanLerpComponent = true;
    StartComponentTransform = CameraMain->GetRelativeTransform();
    EndComponentTransform = DialogueCamera->GetRelativeTransform();
	
	// par rapport à sa finale position dans le monde
	
	// A : position world finale du bp = EndActorPosition
	// B : relative pos (de dialogue cam) dans bp =
	FVector DialogueCameraRelativePos = DialogueCamera->GetRelativeLocation();
	// C : relative pos -> world pos par rapport à A
	FVector LookAtLocation =  EndActorTransform.TransformPosition(DialogueCameraRelativePos);
	
	// on definit la rotation world voulue
	// forward = target - look at
	FVector Target = (EndActorPosition + Speaker->GetActorLocation())/2; // (middle + Speaker) / 2
	FVector Forward = (Target - LookAtLocation).GetSafeNormal();
	FRotator WorldRotation = Forward.Rotation(); // rot world
	
	// on convert rot world en relative au bp
	FQuat RelativeRot = EndActorTransform.InverseTransformRotation(WorldRotation.Quaternion());
	// on set relative rot au EndComponentTransform
	EndComponentTransform.SetRotation(RelativeRot);
	
	// STATE
	PreviousState = CameraState;
    NextState = ECameraState::DialogueCamera;
	
	// Begin lerp in tick
	IsSettingCamera = true;
	
}


void UCameraWorldSubsystem::LerpCamera(float DeltaTime)
{
	LerpTimer += DeltaTime * 0.5f;

	//Lerp Camera Actor
	if (CanLerpActor)
		LerpCameraActor(DeltaTime); // World

	//Lerp Camera Component
	if (CanLerpComponent)
		LerpCameraComponent(DeltaTime); // relative to bp

	// On finished lerp
	if (LerpTimer>=1)
	{
		// DEBUG
		//UE_LOG(LogTemp, Display, TEXT("Fini de lerp"));

		FinishCameraLerp();
		
		ResetLerp();
	}
}

void UCameraWorldSubsystem::LerpCameraComponent(float DeltaTime)
{
	// Lerp Camera Component
	FVector NewPos = FMath::Lerp(StartComponentTransform.GetLocation(),EndComponentTransform.GetLocation(),LerpTimer);
	FRotator NewRot = FMath::Lerp(StartComponentTransform.Rotator(),EndComponentTransform.Rotator(),LerpTimer);
	CameraMain->SetRelativeLocationAndRotation(NewPos, NewRot);
	//CameraMain->SetRelativeLocation(NewPos);
}

void UCameraWorldSubsystem::LerpCameraActor(float DeltaTime)
{
	// Lerp Camera Actor
	FVector NewPos = FMath::Lerp(StartActorTransform.GetLocation(),EndActorTransform.GetLocation(),LerpTimer);
	//FRotator NewRot = FMath::Lerp(StartActorTransform.Rotator(),EndActorTransform.Rotator(),LerpTimer);
	//CameraMain->GetOwner()->SetActorLocationAndRotation(NewPos, NewRot);
	CameraMain->GetOwner()->SetActorLocation(NewPos);
}


void UCameraWorldSubsystem::ResetLerp()
{
	IsSettingCamera = false; // stop lerp
	IsZooming = false;
	
	ResetLerpTimer();
}

void UCameraWorldSubsystem::ResetLerpTimer()
{
	LerpTimer = 0; 
}

void UCameraWorldSubsystem::FinishCameraLerp()
{
	CameraState = NextState;
		
	switch (CameraState)
	{
	case ECameraState::GlobalCamera :
		FinishGlobalCameraLerp();
		break;
			
	case ECameraState::DialogueCamera:
		FinishDialogueCameraLerp();
		break;
			
	case ECameraState::MusicCamera:
		FinishMusicCameraLerp();
		break;
		
	default :
		UE_LOG(LogTemp, Warning, TEXT("SwitchCamera is not a valid"));
		break;
	}
}

void UCameraWorldSubsystem::FinishDialogueCameraLerp()
{
	
}

void UCameraWorldSubsystem::FinishMusicCameraLerp()
{
}

void UCameraWorldSubsystem::FinishGlobalCameraLerp()
{
	UGlobalGameSubsystem* GlobalGameSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalGameSubsystem>();
	
	// Pipou IDLE
	for (APipouCharacter* PipouCharacter : GlobalGameSubsystem->PipouCharacters)
	{
		PipouCharacter->StateMachine->ChangeState(EPipouCharacterStateID::Idle);
	}
}


AActor* UCameraWorldSubsystem::FindCameraActorByTag(const FName& Tag) const
{
	//Return actor by tag

	TArray<AActor*> Cameras;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), Tag,Cameras);
	
	AActor* CameraActor = nullptr;

	// Get Cam Actor 
	if (Cameras.Num() > 0)
	{
		CameraActor =  Cameras[0];
	}

	return CameraActor;
}

UCameraComponent* UCameraWorldSubsystem::FindCameraComponentByTag(const AActor* Parent, const FName& Tag) const
{
	UCameraComponent* CameraComponent = nullptr;
	
	//Find Camera in child components
	TArray<UActorComponent*> Components =  Parent->GetComponentsByTag(UCameraComponent::StaticClass(),Tag);
	if (Components.Num() != 0)
	{
		CameraComponent = Cast<UCameraComponent>(Components[0]);
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

void UCameraWorldSubsystem::GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax) const
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

bool UCameraWorldSubsystem::ShouldSkipCameraForThisMap() const
{
	const UWorld* World = GetWorld();
	if (!World) return true;

	const FString LevelName = UGameplayStatics::GetCurrentLevelName(World, true);

	return LevelName.Equals(TEXT("CutsceneScene"), ESearchCase::IgnoreCase)
		|| LevelName.Equals(TEXT("TrueMainMenu"), ESearchCase::IgnoreCase)
		|| LevelName.Equals(TEXT("MainMenu"), ESearchCase::IgnoreCase);
}