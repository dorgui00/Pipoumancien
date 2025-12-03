// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HUDData.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UHUDData : public UDataAsset
{
	GENERATED_BODY()

public :
	UPROPERTY(EditAnywhere, Category="Note Images")
	UImage* NoteUp;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UImage* NoteRight;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UImage* NoteDown;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UImage* NoteLeft;
};
