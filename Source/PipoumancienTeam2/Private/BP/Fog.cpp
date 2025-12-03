// Fill out your copyright notice in the Description page of Project Settings.


#include "BP/Fog.h"
#include "Data/F_Skeleton.h"


// Sets default values
AFog::AFog()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Cube = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cube"));
	RootComponent = Cube;
}

void AFog::SupprimerFog(F_Skeleton* Skeleton)
{
	if (IDSkeleton==Skeleton->ID)
	{
		if (DynMaterial)
		{
			// Équivaut à "Set Scalar Parameter Value" (paramètre : Alfa)
			DynMaterial->SetScalarParameterValue(TEXT("Alfa"), 0.0f);
		}
	}
}

// Called when the game starts or when spawned
void AFog::BeginPlay()
{
	Super::BeginPlay();
	// Création de l’instance dynamique au début ou quand tu veux
	if (FogMaterial && Cube)
	{
		DynMaterial = UMaterialInstanceDynamic::Create(FogMaterial, this);
		Cube->SetMaterial(0, DynMaterial);
	}
	
}

// Called every frame
void AFog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

