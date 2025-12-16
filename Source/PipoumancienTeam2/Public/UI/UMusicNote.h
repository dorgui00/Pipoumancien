// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "NiagaraSystemWidget.h"
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
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ---- NOTE TEXTURE IMAGE ----
	UPROPERTY(meta = (BindWidget))
	UImage* NoteImage;

	UPROPERTY()
	UCanvasPanelSlot* NoteImageSlot;

	// ---- VALIDATION NOTE FEEDBACK ----
	FVector2D NoteCurrentValidationScale = {32, 32 };
	FVector2D NoteTargetValidationScale;
	FVector2D NoteValidationFinalScale;

	float NoteImageCurrentOpacity = 1.f;
	float NoteImageTargetOpacity;

	bool IsPlayingValidation = false;

	UPROPERTY()
	float InterpValidationSpeedScale = 10.f;

	UPROPERTY()
	float InterpSpeedOpacity = 5.f;

	void PlayValidationNote(FVector2D NewTargetSize, float NewOpacity);


	// ---- FAIL NOTE FEEDBACK ----
	float NoteCurrentRotation = 1;
	float NoteTargetRotation = 0;
	float InterpRotationSpeed = 50.f;
	
	FVector2D NoteCurrentFailScale = {50, 50 };
	FVector2D NoteTargetFailScale;
	float InterpFailScaleSpeed = 30.f;

	bool IsPlayingFailAnimation = false;

	void PlayFailNote();

	
	// ---- IMAGE NOTE ----
	void SetNoteTexture(UTexture2D* NewTexture);


	// ---- NOTE SPAWN POINT ----
	UPROPERTY()
	UCanvasPanelSlot* SpawnPoint;

	UPROPERTY()
	UCanvasPanelSlot* EndPoint;

	// ---- NIAGARA FEEDBACK ----
	UPROPERTY(meta = (BindWidget))
	UNiagaraSystemWidget* NiagaraFailNote;
	

private:
	// ---- VALIDATION NOTE FEEDBACK ----
	void Internal_PlayValidationNote(float DeltaTime);

	// ---- FAIL NOTE FEEDBACK ----
	void Internal_PlayFailNote(float DeltaTime);
	
};
