// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AssignActorComponent.generated.h"

class UBushVFX;
class UNiagaraSystem;
class AActor;

USTRUCT(BlueprintType)
struct FActorsData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	AActor* Actor = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Actors")
	TSubclassOf<UPrimitiveComponent> ComponentClass;
};

UCLASS()
class PIPOUMANCIENTEAM2_API AAssignActorComponent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssignActorComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, Category = "Actor Component Assigner")
	void ScanForActor();

	UFUNCTION(CallInEditor, Category = "Actor Component Assigner")
	void AssignComponents();

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	TSubclassOf<UBushVFX> ComponentToAssign;

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	UNiagaraSystem* FXToAssign;

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	TSubclassOf<AActor> PhantomClassToAssign;

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	TSubclassOf<AActor> NecroClassToAssign;

	FString GetActorIdForMatching(AActor* Actor) const;

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	FString ActorNamePrefix = TEXT("MOD_Bush");

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	bool bScanOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Actor Component Assigner")
	bool bTickInEditor = true;

	UPROPERTY(VisibleAnywhere, Category = "Tree Sway Tool")
	TArray<FActorsData> Actors;
};
