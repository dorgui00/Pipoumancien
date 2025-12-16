// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/EngineTypes.h"
#include "Components/WidgetComponent.h"
#include "SkeletonController.generated.h"

class UAC_SkeletonFollower;
class UAC_SetAnimations;
class UUIDialoge;
class UUIMerci;
class UGlobalDataTableSubsystem;
class UDataTableGameInstanceSubsystem;
class ADB_Manager;
class UAnimationAsset;
class USkeletalMeshComponent;
struct F_Skeleton;
class AFog;
class UPhysicalMaterial;
class UNiagaraSystem;
class UMyAnimNotify_PlayCleanseOnce;

enum class ESkeletonState : uint8{
	None = 0,
	Dead = 1,
	Transport = 2,
	BackToHome = 3, // follow spline to go back home
	Dialogue = 4, // reached his home
};


UCLASS()
class PIPOUMANCIENTEAM2_API ASkeletonController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletonController();
	virtual ~ASkeletonController() override;
	
	// Override
	virtual void Tick(float DeltaTime) override;

	//Skeleton Controller
	F_Skeleton* MySkeleton;

	UFUNCTION(BlueprintCallable)
	F_Skeleton GetMyData() const;

	// STATE
	ESkeletonState GetState() const;
	
	// Transport
	void SetSkeletonForTransport();

	//Village
	UFUNCTION()
	void OnEnterVillage();  // in BackToHome state
	
	// Dialogue
	void SetSkeletonForDialogue(); // in dialogue state

	UFUNCTION()
	void OnReachHome();


	// --- VISUALS ---
#pragma region Visuals
	
	// Mesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	USkeletalMeshComponent* TargetMesh = nullptr;

	// Anims
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* IdleAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* WalkAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* WaitAnimation = nullptr;
	
	// Wake Anim
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	TArray<UAnimationAsset*> WakeAnims ;

	//walk speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Variables")
	float SkeletonMovespeed = 200.f;

	// UPROPERTY()
	// TObjectPtr<UAnimationAsset> WakeAnimToPlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int CurrentAnimationIndex = 0;
	
	UFUNCTION(BlueprintImplementableEvent)
	void PlayWakeAnim();

	UFUNCTION(BlueprintImplementableEvent)
	void EndFeedbacksOfMelody(bool HasSucceed);

	UFUNCTION(BlueprintImplementableEvent)
	void StartFeedbacksOfMelody();
		
#pragma endregion
	
	// --- GET SET ---

	UFUNCTION(BlueprintCallable, Category = "Skeleton|Follow")
	void SetFollowerSplineFollowSpeed(float NewSpeed);

	UFUNCTION(BlueprintCallable, Category = "Skeleton|Follow")
	float GetFollowerSplineFollowSpeed() const;


	// --- UI ---
	void OpenDialogue();

	void InterationDialogue();
	void InterationDialoguenOFF();

	UPROPERTY()
	UUIDialoge* PlayerWidget;
	
	UPROPERTY()
	UUIMerci* PlayerWidgetMerci;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UWidgetComponent* WidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UWidgetComponent* WidgetComponentMerci;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	AActor* ZoneBird;
	
	// --- SOUNDS ---
	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> FootstepCue;

	// --- VFX ---
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	float CleanseWindowDuration = 1.0f;

	bool bCleanseWindowActive = false;
	bool bCleanseFXPlayed = false;

	FTimerHandle CleanseWindowTimerHandle;

	UFUNCTION()
	void StartCleanseWindow();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, Category="Skeleton")
	int ID = 0;

	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UStaticMeshComponent* ZoneVictoirMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUIDialoge> PlayerWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUIMerci> PlayerWidgetMerciClass;
	
	UPROPERTY()
	bool isDialoge = true;

	bool isDialogVisible = false;

	UFUNCTION()
	void BeginOverlaps(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	UFUNCTION()
	void EndOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

	int ValutFrase = 0;

	// Liste de Fog (équivalent du tableau dans le Blueprint)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Références")
	TArray<AFog*> FogList; 
	
private :
	
	// STATE
	ESkeletonState MyState = ESkeletonState::Dead;

	// FOLLOW
	UPROPERTY()
	TObjectPtr<UAC_SkeletonFollower> FollowComponent;

	// ANIM
	FVector LastLocation = FVector::ZeroVector;
	bool bHasLastLocation = false;
	bool bWasMoving = false;
	bool bWasFollowing = false;

	void PlayIdle();
	void PlayWalk();
	void PlayWait();
	void UpdateAnimation(float DeltaTime);
	void FogDilet();

	void InitMyVisuals();

	float FootstepTimer = 0.f;

	// SOUNDS
	UPROPERTY(EditAnywhere, Category = "Audio")
	float FootstepInterval = .6f;

	void PlayFootstepsSound(UPhysicalMaterial* PhysMat);

	//niagara.
	UFUNCTION()
	void EndCleanseWindow();
};
