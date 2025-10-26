// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/SplineComponent.h"
#include "AC_SkeletonFollower.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIPOUMANCIENTEAM2_API UAC_SkeletonFollower : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAC_SkeletonFollower();

	UPROPERTY(EditAnywhere)
	FName PlayersTag = "PipouPlayer";
	
	TArray<AActor*> PipouPlayers;
	AActor* ParentActor;

	const float FollowRange = 300.f;
	float MoveSpeed = 10.f;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bPlayer1InRange = false;
	bool bPlayer2InRange = false;
	FVector Player1Location;
	FVector Player2Location;
	bool bStartFollowing = false;

	UFUNCTION()
	void OnParentHit(AActor* SelfActor, AActor* OtherActor, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnParentOverlap(AActor* OverlappedActor, AActor* OtherActor);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void CheckPlayerRange();

	void FollowPlayers(float DeltaTime);

};
