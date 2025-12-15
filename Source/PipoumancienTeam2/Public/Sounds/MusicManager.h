// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Character/PipouCharacter.h"
#include "MusicManager.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API AMusicManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMusicManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
    USoundBase* MainMusicSound;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
    USoundBase* VillageMusicSound;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music")
    UAudioComponent* MainMusicAudioComp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music")
    UAudioComponent* VillageMusicAudioComp;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Fade")
    float MainFadeInTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Fade")
    float MainFadeOutTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Fade")
    float VillageFadeInTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Fade")
    float VillageFadeOutTime = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music")
    bool bStartWithMainMusic = true;

    //trigger
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Trigger")
    AActor* VillageTriggerActor;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Music|Trigger")
    TSubclassOf<AActor> ActorClassToTriggerOn;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Music")
	void EnterVillageMusic();

	UFUNCTION(BlueprintCallable, Category = "Music")
	void ReturnToMainMusic();

private:

    int32 NumPlayersInTrigger = 0;

    void BindToTrigger();

    UFUNCTION()
    void OnTriggerBeginOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex,
        bool bFromSweep,
        const FHitResult& SweepResult
    );

    UFUNCTION()
    void OnTriggerEndOverlap(
        UPrimitiveComponent* OverlappedComp,
        AActor* OtherActor,
        UPrimitiveComponent* OtherComp,
        int32 OtherBodyIndex
    );
};
