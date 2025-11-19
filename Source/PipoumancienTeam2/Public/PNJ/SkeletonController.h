// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SkeletonController.generated.h"

class UUIDialoge;
class UGlobalDataTableSubsystem;
class UDataTableGameInstanceSubsystem;
class ADB_Manager;
struct F_Skeleton;

UCLASS()
class PIPOUMANCIENTEAM2_API ASkeletonController : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASkeletonController();

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
	UUIDialoge* PlayerWidget;
	
	UPROPERTY()
	bool isDialoge = true;

	UFUNCTION()
	void BeginOverlaps(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	// UFUNCTION()
	// void EndOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	// 	UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

	int ValutFrase = 0;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	F_Skeleton* MySkeleton;

	void SetSkeletonForTransport();
	
};
