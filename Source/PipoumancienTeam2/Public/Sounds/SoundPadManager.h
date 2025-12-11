// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoundPadManager.generated.h"

class ATriggerBox;
class UAudioComponent;
class USoundBase;


UCLASS()
class PIPOUMANCIENTEAM2_API ASoundPadManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ASoundPadManager();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Audio")
	UAudioComponent* AudioComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	ATriggerBox* VillageTrigger;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<ACharacter> NecroCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
	TSubclassOf<ACharacter> PhantomCharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* OutsidePad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sound")
	USoundBase* VillagePad;

	UFUNCTION()
	void OnVillageBeginOverlap(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnVillageEndOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void UpdatePad();
	void PlayPad(USoundBase* NewPad);

	UPROPERTY()
	ACharacter* NecroPlayer;

	UPROPERTY()
	ACharacter* PhantomPlayer;

	bool bNecroInVillage;
	bool bPhantomInVillage;
};
