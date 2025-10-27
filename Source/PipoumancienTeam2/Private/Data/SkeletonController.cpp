// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/SkeletonController.h"

#include "Data/DB_Manager.h"


// Sets default values
ASkeletonController::ASkeletonController()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ASkeletonController::BeginPlay()
{
	Super::BeginPlay();

	DB_Manager = ADB_Manager::Instance();

	if (DB_Manager)
	MySkeleton = DB_Manager->GetSkeletonByID(ID);
}

// Called every frame
void ASkeletonController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

