// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

struct FInvisibleObject;
class APipouCharacter;
class ASkeletonController;
class UCameraComponent;
/**
 * 
*/

// UENUM()
enum class ECameraState : uint8{
	None = 0,
	MusicCamera = 1,
	GlobalCamera = 2,
	DialogueCamera = 3,
};

UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region SubsystemsOverride
public :
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};

#pragma endregion

#pragma region Init
public :
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;
	
	void InitCameraSubsystem();
	
private :
	// Init Cameras
	void AssignAllCameras();
	
	void InitMainCamera();

#pragma endregion

#pragma region FollowTargets
public :
	void AddFollowTarget(UObject* FollowTarget);
	
	void RemoveFollowTarget(UObject* FollowTarget);
	
protected:
	UPROPERTY()
	TArray<UObject*> FollowTargets;
	
	FVector CalculateAveragePositionBetweenTargets();

	float CalculateGreatestDistanceBetweenTargets();
	
#pragma endregion

#pragma region Frustrum
public :
	bool ClampPositionInsideQuad(const FVector& InPos, FVector& OutPos); // in chara state
	
protected :
	TArray<FVector> GetCameraQuadGroundBounds();
#pragma endregion
	
#pragma region Misc
protected:
	AActor* FindCameraActorByTag(const FName& Tag) const;
	UCameraComponent* FindCameraComponentByTag(const AActor* Owner, const FName& Tag) const;

private :
	void InitCameraRotationToPivot();
	
#pragma endregion

#pragma region Bounds
	
private :
	UPROPERTY()
	FVector2D CameraBoundsMin;

	UPROPERTY()
	FVector2D CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	AActor* FindCameraBoundsActor();

	void InitCameraBounds(AActor* CameraBoundsActor);

	void ClampPositionIntoCameraBounds(FVector& Position);

	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax) const;

	FVector CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition);
	
#pragma endregion

#pragma region Zoom
	
private  :
	// Zoom according to distance
	UPROPERTY()
	float CameraZoomYMin = 0.f;

	UPROPERTY()
	float CameraZoomYMax = 0.f;

	UPROPERTY()
	float CameraZoomDistanceBetweenTargetsMin = 300.f;

	UPROPERTY()
	float CameraZoomDistanceBetweenTargetsMax = 1500.f;

	UFUNCTION()
	void InitCameraZoomParameters();
	
	void TickUpdateCameraZoom(float DeltaTime);

	// zoom / dezoom
public :

	UFUNCTION(BlueprintCallable) 
	void Zoom(float Value);
	
	UFUNCTION(BlueprintCallable) 
	void Dezoom(float Value);

	UFUNCTION(BlueprintCallable) 
	bool GetIsZoomed();

	void SkeletonInteractionZoom(bool Zoom);
	float SkeletonZoom = 115.f;
	
private :
	bool IsZoomed = false;
	
#pragma endregion

#pragma  region Visible Target
public :
	
	void AddVisibleTarget(UObject* VisibleTarget);
	void RemoveVisibleTarget(UObject* VisibleTarget);
	
private :
	UPROPERTY()
	TArray<UObject*> VisibleTargets;
	void SetVisibleTarget(UObject* VisibleTarget);
	
	void InitCameraVisibility();
	void TickUpdateCameraVisibility(float DeltaTime);

	void SetCloakingObjectBehaviour(const FHitResult& Hit); // object that hides visible target
	void MakeObjectVisibleAgain(TObjectPtr<AActor> InvisibleObject); // no more invisible
	
	void CompareCurrentFromPreviousInvisibleObjects();


	UPROPERTY()
	UMaterialInterface* InvisibleMaterial;
	
	UPROPERTY()
	TArray<FInvisibleObject> InvisibleObjects;
	//TMap<TObjectPtr<AActor>, TObjectPtr<UMaterialInterface>> InvisibleObjects;

	UPROPERTY()
	TArray<AActor*> CurrentCloakingObjects;


	
#pragma endregion

	// CAMERAS
#pragma region Cameras

public :
	
	// STATE
	ECameraState GetState() const;
	
private :
	// STATE
	ECameraState CameraState;
	ECameraState PreviousState;
	ECameraState NextState = ECameraState::GlobalCamera;
	
	// LERP CAMERAS
	bool IsSettingCamera = false;
	bool IsZooming = false;
	float LerpTimer = 0;
	
	void LerpCamera(float DeltaTime);
	void LerpCameraComponent(float DeltaTime);
	void LerpCameraActor(float DeltaTime);

	// Component Pos/Rot
	FTransform StartComponentTransform;
	FTransform EndComponentTransform;

	// Actor Pos/Rot
	FTransform StartActorTransform;
	FTransform EndActorTransform;

	bool CanLerpActor = false;
	bool CanLerpComponent = false;
	
	void ResetLerp();
	void ResetLerpTimer();
	void FinishCameraLerp();
	

#pragma endregion

	// GLOBAL CAMERA
#pragma region Global Camera
	
public :
	void SetGlobalCamera();

private :
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> GlobalCamera;
	
	void TickUpdateCameraPosition(float DeltaTime);
	
	void FinishGlobalCameraLerp();
	
	
#pragma endregion

	//MUSIC CAMERA
#pragma region MusicCamera

public :
	void SetMusicCamera();

private :	

	UPROPERTY()
	TObjectPtr<UCameraComponent> MusicCamera;
	
	void FinishMusicCameraLerp();

#pragma endregion

	// DIALOGUE CAMERA
#pragma region Dialogue Camera
public :

	void SetDialogueCamera(const APipouCharacter* Interactor, ASkeletonController* Speaker);

private :
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> DialogueCamera;
	
	void FinishDialogueCameraLerp();
	
# pragma endregion
};




