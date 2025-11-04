// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "USlot.generated.h"

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

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Textures")
	UTexture2D* Texture;

	UPROPERTY()
	EMusicNoteType SlotNote = EMusicNoteType::None;
};
