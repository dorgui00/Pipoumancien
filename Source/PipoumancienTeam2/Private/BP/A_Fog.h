// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "A_Fog.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API AA_Fog : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AA_Fog();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UStaticMeshComponent* Cube;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UMaterialInstanceDynamic* DynMaterial;
	void ChangeValue(float valut);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
