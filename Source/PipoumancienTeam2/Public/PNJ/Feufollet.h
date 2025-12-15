// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Feufollet.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UMaterialInstanceDynamic;
class APipouCharacter;

UCLASS()
class PIPOUMANCIENTEAM2_API AFeufollet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFeufollet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* CollisionSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flee")
	float FleeSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flee")
	TSubclassOf<ACharacter> PhantomCharacterClass;

	UPROPERTY(EditAnywhere, Category = "Movement|Bob")
	float BobAmplitude = 20.f;

	UPROPERTY(EditAnywhere, Category = "Movement|Bob")
	float BobFrequency = 2.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Flee")
	TSubclassOf<ACharacter> NecroCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
	float FadeDelay = 5.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
	float FadeDuration = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fade")
	FName OpacityParameterName = "Opacity";



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:

	bool bIsFleeing = false;

	bool bIsFading = false;

	FVector FleeDirection = FVector::ZeroVector;

	UMaterialInstanceDynamic* DynamicMaterial = nullptr;

	FTimerHandle FadeStartTimerHandle;

	float FadeElapsedTime = 0.0f;

	//bob
	float BobTime = 0.f;
	float LastBobOffset = 0.f;

	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void StartFade();
};
