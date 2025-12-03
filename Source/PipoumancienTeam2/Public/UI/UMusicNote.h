// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UMusicNote.generated.h"

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
class PIPOUMANCIENTEAM2_API UMusicNote : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UImage* NoteImage;

	void SetNoteTexture(UTexture2D* NewTexture);

	UPROPERTY()
	UCanvasPanelSlot* SpawnPoint;

	UPROPERTY()
	UCanvasPanelSlot* EndPoint;

};
