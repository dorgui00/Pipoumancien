// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Engine/EngineTypes.h"
#include "SkeletonController.generated.h"

class UAC_SkeletonFollower;
class UAC_SetAnimations;
class UUIDialoge;
class UGlobalDataTableSubsystem;
class UDataTableGameInstanceSubsystem;
class ADB_Manager;
class UAnimationAsset;
class USkeletalMeshComponent;
struct F_Skeleton;
class AFog;
class UPhysicalMaterial;


enum class ESkeletonState : uint8{
	None = 0,
	Dead = 1,
	Transport = 2,
	BackToHome = 3, // follow spline to go back home
	Dialogue = 3, // reached his home
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

	void OpenDialogue();
	
	UPROPERTY()
	UUIDialoge* PlayerWidget;

	//ANIM
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* IdleAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* WalkAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	UAnimationAsset* WaitAnimation = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PNJ|Animations")
	USkeletalMeshComponent* TargetMesh = nullptr;

	//SOUNDS
	UPROPERTY(EditAnywhere, Category = "Audio")
	TObjectPtr<USoundBase> FootstepCue;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category="Skeleton")
	int ID = 0;

	// Called when the game starts or when spawned
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UStaticMeshComponent* ZoneVictoirMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<UUIDialoge> PlayerWidgetClass;
	
	UPROPERTY()
	bool isDialoge = true;

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


	float FootstepTimer = 0.f;

	UPROPERTY(EditAnywhere, Category = "Audio")
	float FootstepInterval = .6f;

	void PlayFootstepsSound(UPhysicalMaterial* PhysMat);

};
