// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "USlot.generated.h"

class UTextBlock;
class UCanvasPanelSlot;
class UCanvasPanel;

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

	UPROPERTY()
	UCanvasPanelSlot* SpawnPoint;

	UPROPERTY()
	UCanvasPanelSlot* EndPoint;

	float NoteAlpha = 0;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LetterText;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture;

	UPROPERTY()
	EMusicNoteType SlotNote = EMusicNoteType::None;

	TMap<EMusicNoteType, FText> SlotTextures
	{
		{ EMusicNoteType::A, FText::FromString(TEXT("A")) },
		{ EMusicNoteType::B, FText::FromString(TEXT("B")) },
		{ EMusicNoteType::Y, FText::FromString(TEXT("Y")) },
		{ EMusicNoteType::X, FText::FromString(TEXT("X")) }
	};

	FText GetNameFromMusicNoteType(EMusicNoteType NoteType);
};
