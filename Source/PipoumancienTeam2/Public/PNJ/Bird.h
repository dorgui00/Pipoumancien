// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Game/GlobalGameSubsystem.h"
#include "GameFramework/Actor.h"
#include "Bird.generated.h"

class UBirdWidget;
class ASkeletonController;
class UWidgetComponent;

UCLASS()
class PIPOUMANCIENTEAM2_API ABird : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABird();

	void SetWidgetVisibility(bool Visibility);
	
	void SetMyNotes() const; // update bird notes

	void SetWidgetINVisible();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// --- UI ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* WidgetComponent;

private :
	// --- INIT ---
	void Init();

	// --- UI ---
	UPROPERTY()
	UBirdWidget* BirdWidget;
	
	

	// --- UTILITES ---
	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	UPROPERTY()
	UGlobalHUDSubsystem* GlobalHUDSubsystem;

};
