// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BushVFX.generated.h"

class USphereComponent;
class UNiagaraSystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PIPOUMANCIENTEAM2_API UBushVFX : public UActorComponent
{
	GENERATED_BODY()

public:
	UBushVFX();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Transient)
	USphereComponent* TriggerSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Trigger")
	float SphereRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX")
	UNiagaraSystem* EnterFX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FX", meta = (ClampMin = "0.0"))
	float Cooldown;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Characters")
	TSubclassOf<AActor> PhantomCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Characters")
	TSubclassOf<AActor> NecroCharacterClass;

	bool bCanTrigger;

	FTimerHandle CooldownTimerHandle;

	UFUNCTION()
	void OnSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	void ResetCooldown();
};