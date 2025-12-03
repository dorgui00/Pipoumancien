// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PNJ/SkeletonController.h"
#include "Fog.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API AFog : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AFog();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Le mesh sur lequel appliquer le matériel
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Cube;

	// Matériau de base (Fog)
	UPROPERTY(EditAnywhere)
	UMaterialInterface* FogMaterial;

	// Instance dynamique
	UPROPERTY()
	UMaterialInstanceDynamic* DynMaterial;
	
	// Fonction équivalente à ton Custom Event "Suprimer Fog"
	void SupprimerFog(F_Skeleton* Skeleton);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere)
	int IDSkeleton;
	
};
