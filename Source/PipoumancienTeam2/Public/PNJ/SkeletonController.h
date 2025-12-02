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
struct F_Skeleton;


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
	
private :
	
	// STATE
	ESkeletonState MyState = ESkeletonState::Dead;

	// FOLLOW
	UPROPERTY()
	TObjectPtr<UAC_SkeletonFollower> FollowComponent;

	// ANIM
	UPROPERTY()
	TObjectPtr<UAC_SetAnimations> AnimComponent;

	UPROPERTY(EditAnywhere, Category = "Animations")
	FComponentReference TargetMesh;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* IdleAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WalkAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WaitAnim;
};
