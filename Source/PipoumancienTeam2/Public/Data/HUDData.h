// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystemWidget.h"
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
	UTexture2D* NoteUp;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UTexture2D* NoteRight;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UTexture2D* NoteDown;

	UPROPERTY(EditAnywhere, Category="Note Images")
	UTexture2D* NoteLeft;

	UPROPERTY(EditAnywhere, Category="Note Images Interaction")
	UTexture2D* NoteUpInteraction;

	UPROPERTY(EditAnywhere, Category="Note Images Interaction")
	UTexture2D* NoteRightInteraction;

	UPROPERTY(EditAnywhere, Category="Note Images Interaction")
	UTexture2D* NoteDownInteraction;

	UPROPERTY(EditAnywhere, Category="Note Images Interaction")
	UTexture2D* NoteLeftInteraction;

	UPROPERTY(EditAnywhere, Category="Material Mistake")
	UMaterialInterface* MistakeMaterialInstance;

	UPROPERTY(EditAnywhere, Category="Feedback UI")
	UNiagaraSystem* WinFeedback;

	UPROPERTY(EditAnywhere, Category="Feedback UI")
	UNiagaraSystem* LoseFeedback;
	
};
