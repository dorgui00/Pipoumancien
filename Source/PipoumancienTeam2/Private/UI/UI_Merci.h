// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "PNJ/SkeletonController.h"
#include "UI_Merci.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UUIMerci : public UUserWidget
{
	GENERATED_BODY()


public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typewriter")
	FString FullText;

	void SetMerci(F_Skeleton* Skeleton);

	virtual void NativeConstruct() override;
	
protected:
	// L’ID de la ligne de dialogue à afficher
	UPROPERTY(EditAnywhere, Category = "Dialog")
	int32 ID;
	
	FString CurrentDialogue;


private:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Trenks;
	
};
