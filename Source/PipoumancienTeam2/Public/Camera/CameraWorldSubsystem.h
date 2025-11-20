// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "CameraWorldSubsystem.generated.h"

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

UCLASS()
class PIPOUMANCIENTEAM2_API UCameraWorldSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

#pragma region SubsystemsOverride
public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void PostInitialize() override;
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override {return TStatId();};

#pragma endregion

#pragma region Init
public :
	
	UPROPERTY()
	TObjectPtr<UCameraComponent> CameraMain;

	void OnPostLoadMap(UWorld* LoadedWorld);

	//broadcasted in gamemode
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCamerasReady);

	UPROPERTY()
	FOnCamerasReady OnCamerasReady;
	
protected:
	FTransform InitMainCameraTransform;
	
	void TickUpdateCameraZoom(float DeltaTime);
	
	void TickUpdateCameraPosition(float DeltaTime);

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
public :
	void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);

protected :
	UPROPERTY()
	FVector2D CameraBoundsMin;

	UPROPERTY()
	FVector2D CameraBoundsMax;

	UPROPERTY()
	float CameraBoundsYProjectionCenter;

	AActor* FindCameraBoundsActor();

	void InitCameraBounds(AActor* CameraBoundsActor);

	void ClampPositionIntoCameraBounds(FVector& Position);

	//void GetViewportBounds(FVector2D& OutViewportBoundsMin, FVector2D& OutViewportBoundsMax);

	FVector CalculateWorldPositionFromViewportPosition(const FVector2D& ViewportPosition);
	
#pragma endregion

#pragma region Zoom
	protected :
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
	
#pragma endregion

	
#pragma region MusicCamera
public :
	// Lerp cameras

	ECameraState CameraState;
	ECameraState PreviousState;

	void SetMusicCamera();

	void SetGlobalCamera();

protected :
	
	bool IsSettingCamera = false;

	FTransform StartComponentTransform;
	FTransform EndComponentTransform;

	FTransform StartActorTransform;
	FTransform EndActorTransform;

	bool CanLerpActor = false;
	bool CanLerpComponent = false;
	
	void FinishCameraLerp();

	void FinishDialogueCameraLerp();
	void FinishMusicCameraLerp();
	void FinishGlobalCameraLerp();

	void AssignAllCameras();
	void InitMainCamera();

	UPROPERTY()
	TObjectPtr<UCameraComponent> GlobalCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> MusicCamera;

	UPROPERTY()
	TObjectPtr<UCameraComponent> DialogueCamera;
	
#pragma endregion

#pragma region Dialogue Camera
public :
	void SetDialogueCamera(APipouCharacter* Interactor, ASkeletonController* Speaker);
	
	
	void LerpCamera(float DeltaTime);
	void LerpCameraComponent(float DeltaTime);
	void LerpCameraActor(float DeltaTime);

	float LerpTimer = 0;
	
# pragma endregion
};
