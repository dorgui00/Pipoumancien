// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MusicGenericData.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UMusicGenericData : public UDataAsset
{
	GENERATED_BODY()
	
public :
	
	// SOUNDS
	UPROPERTY(EditAnywhere, Category="Sound")
	TObjectPtr<USoundCue> FailedNoteSound;
	
};
