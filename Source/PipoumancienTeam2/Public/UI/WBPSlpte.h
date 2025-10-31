// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "WBPSlpte.generated.h"


UENUM(BlueprintType)
enum class E_Musique : uint8
{
	Note1 UMETA(DisplayName="Note 1"),
	Note2 UMETA(DisplayName="Note 2"),
	Note3 UMETA(DisplayName="Note 3"),
	Note4 UMETA(DisplayName="Note 4")
};
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UWBPSlote : public UUserWidget
{
	GENERATED_BODY()

public:

	
	UPROPERTY(meta = (BindWidget))
	UImage* Image_012;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Slote;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SloteDefette;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* SloteVictoir;
	
	UFUNCTION(BlueprintCallable)
	void ChoitMusique(E_Musique Musique);


protected:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture5;

	virtual void NativeConstruct() override;
};
