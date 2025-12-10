// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMusicNote.h"

#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "UI/GlobalHUDSubsystem.h"

void UMusicNote::NativeConstruct()
{
	Super::NativeConstruct();
	NoteImageSlot = Cast<UCanvasPanelSlot>(NoteImage->Slot);
	NoteCurrentSize = {32, 32 };
	NoteImageCurrentOpacity = 1.f;
}

void UMusicNote::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!IsPlayingValidation || !NoteImageSlot) return;

	NoteCurrentSize = FMath::Vector2DInterpTo(NoteCurrentSize, NoteTargetSize, InDeltaTime, InterpSpeedSize);
	NoteImageSlot->SetSize(NoteCurrentSize);

	NoteImageCurrentOpacity = FMath::FInterpTo(NoteImageCurrentOpacity, NoteImageTargetOpacity, InDeltaTime, InterpSpeedOpacity);
	NoteImage->SetColorAndOpacity({ GetColorAndOpacity().R, 1, GetColorAndOpacity().B, NoteImageCurrentOpacity });

	if (NoteTargetSize == NoteFinalSize * 0.5f && FVector2D::Distance(NoteCurrentSize, NoteTargetSize) < 1.f)
	{
		NoteTargetSize = NoteFinalSize * 1.3f;
		InterpSpeedSize = 12.f;
	}
	else if (NoteTargetSize == NoteFinalSize * 1.3f && FVector2D::Distance(NoteCurrentSize, NoteTargetSize) < 1.f)
	{
		NoteTargetSize = NoteFinalSize;
		InterpSpeedSize = 8.f;
	}

	// Stop Animation
	bool IsSizeDone = FVector2D::Distance(NoteCurrentSize, NoteFinalSize) < 0.5f;
	bool IsOpacityDone = FMath::Abs(NoteImageCurrentOpacity - NoteImageTargetOpacity) < 0.01f;

	if (IsSizeDone && IsOpacityDone)
	{
		IsPlayingValidation = false;
	}
}

void UMusicNote::PlayValidationNote(FVector2D NewTargetSize, float NewOpacity)
{
	IsPlayingValidation = true;
	
	NoteFinalSize = NewTargetSize;

	NoteTargetSize = NoteFinalSize * 0.5f;
	NoteImageTargetOpacity = NewOpacity;

	InterpSpeedSize = 20.f; 
	InterpSpeedOpacity = 7.f;
}

void UMusicNote::SetNoteTexture(UTexture2D* NewTexture)
{
	UGlobalHUDSubsystem* HUDSub = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UGlobalHUDSubsystem>();
	if (!HUDSub || !NoteImage)
	{
		UE_LOGFMT(LogTemp, Error, "HUDSubsystem or NoteImage is null !");
		return;
	}

	FSlateBrush NoteImageBrush= NoteImage->GetBrush();
	NoteImageBrush.SetResourceObject(NewTexture);
	NoteImage->SetBrush(NoteImageBrush);
}
