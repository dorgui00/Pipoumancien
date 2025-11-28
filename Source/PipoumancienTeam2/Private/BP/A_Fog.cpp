// Fill out your copyright notice in the Description page of Project Settings.


#include "A_Fog.h"


// Sets default values
AA_Fog::AA_Fog()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AA_Fog::BeginPlay()
{
	Super::BeginPlay();
}

void AA_Fog::ChangeValue(float valut , )
{
	UMaterialInterface* Material = Cube->GetMaterial(0);
	if (!Material) return;

	// 2. Créer un Dynamic Material Instance
	DynMaterial = UMaterialInstanceDynamic::Create(Material, this);

	// 3. Appliquer le material dynamic au mesh
	Cube->SetMaterial(0, DynMaterial);

	// 4. Modifier un paramètre scalar
	if (DynMaterial)
	{
		DynMaterial->SetScalarParameterValue(TEXT("MyParam"), valut);
	}
}

// Called every frame
void AA_Fog::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


