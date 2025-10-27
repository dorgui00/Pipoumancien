// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GameManager.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UGameManager : public UObject
{
	GENERATED_BODY()

private :
	static UGameManager* MyInstance;
	
public :
	UGameManager* Instance();
};
