// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "USlot.generated.h"

UENUM()
enum class EMusicNoteType : uint8
{
	None,
	A,
	B,
	Y,
	X
};

UCLASS()
class PIPOUMANCIENTEAM2_API USlot : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* NoteImage;

	void SetSlotNote(EMusicNoteType NewNoteType);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture;

	UPROPERTY()
	EMusicNoteType SlotNote = EMusicNoteType::None;
};
