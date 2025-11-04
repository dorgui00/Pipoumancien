// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkeletonController.generated.h"

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

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	F_Skeleton* MySkeleton;
	
};
