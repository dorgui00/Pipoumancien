// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MusicManager.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UMusicManager : public UObject
{
	GENERATED_BODY()
	
private :
	static UMusicManager* MyInstance;

public :
	static UMusicManager* Instance();
};
