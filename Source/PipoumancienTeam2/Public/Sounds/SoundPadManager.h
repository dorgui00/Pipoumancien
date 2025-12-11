// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundPadManager.generated.h"

class ATriggerBox;
class UAudioComponent;
class USoundBase;


// SoundPadManager.h

UCLASS()
class PIPOUMANCIENTEAM2_API ASoundPadManager : public AActor
{
	GENERATED_BODY()

public:
	ASoundPadManager();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioA;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioB;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Audio")
	float CrossfadeDuration = 1.0f;

	UPROPERTY()
	UAudioComponent* FadingInAudio;

	UPROPERTY()
	UAudioComponent* FadingOutAudio;

	UPROPERTY()
	float CrossfadeTimer = 0.f;

	UPROPERTY()
	bool bIsCrossfading = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* OutsidePad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* VillagePad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	ATriggerBox* VillageTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<ACharacter> NecroCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<ACharacter> PhantomCharacterClass;

	UPROPERTY()
	ACharacter* NecroPlayer;

	UPROPERTY()
	ACharacter* PhantomPlayer;

	bool bNecroInVillage;
	bool bPhantomInVillage;

	UFUNCTION()
	void OnVillageBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnVillageEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void UpdatePad();
	void StartCrossfade(USoundBase* NewPad);
};