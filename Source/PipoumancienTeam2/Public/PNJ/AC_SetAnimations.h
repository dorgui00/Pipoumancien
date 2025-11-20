// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_SetAnimations.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PIPOUMANCIENTEAM2_API UAC_SetAnimations : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAC_SetAnimations();

protected:

	virtual void BeginPlay() override;

public:	

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* IdleAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WalkAnim;

	UPROPERTY(EditAnywhere, Category = "Animations")
	UAnimSequence* WaitAnim;


	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
