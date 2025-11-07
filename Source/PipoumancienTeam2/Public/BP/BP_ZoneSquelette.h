// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "BP_ZoneSquelette.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API ABP_ZoneSquelette : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABP_ZoneSquelette();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	UStaticMeshComponent* ZoneVictoirMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
	USphereComponent* SphereComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ZoneVictoir")
    	UWidgetComponent* WidgetComponent;

	UFUNCTION()
	void BeginOverlaps(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
	UFUNCTION()
	void endOverlaps(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UResurrectionWidget> PlayerHUDClass;

	UPROPERTY()
	class UResurrectionWidget* PlayerHUD;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
