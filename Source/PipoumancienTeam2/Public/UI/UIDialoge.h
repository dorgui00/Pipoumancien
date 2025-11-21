// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PNJ/SkeletonController.h"
#include "UIDialoge.generated.h"

UCLASS()
class PIPOUMANCIENTEAM2_API UUIDialoge : public UUserWidget
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typewriter")
	FString FullText;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Typewriter")
	float TextSpeed = 0.05f;

	void SetDialogue(F_Skeleton* Skeleton, int Valut);

	void GoToNextDialogue();
	
protected:
	
	// L’ID de la ligne de dialogue à afficher
	UPROPERTY(EditAnywhere, Category = "Dialog")
	int32 ID;

	int32 CurrentDialogueIndex = 0;
	
	// 👇 Fonction appelée quand une touche est pressée
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	int32 text = 0;
private :
	
	UPROPERTY()
	TObjectPtr<UDataTable> DT_Music;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* Discution;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* Name;


	FTimerHandle TextTimerHandle;
	int32 CurrentCharIndex = 0;

	/** Fonction appelée à chaque tick du timer */
	void ShowNextCharacter();
	
	//F_Dialoge* GetSkeletonByID(int ID) const;
	virtual void NativeConstruct() override;

	TArray<FString> CurrentDialogue;
	FString CurrentName;
};