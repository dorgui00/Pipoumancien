// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API AGameManager : public AActor
{
	GENERATED_BODY()

private :
	static AGameManager* MyInstance;
	
public :
	AGameManager* Instance();
};
