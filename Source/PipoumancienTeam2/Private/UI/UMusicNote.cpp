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
	NoteCurrentValidationScale = {32, 32 };
	NoteImageCurrentOpacity = 1.f;
}

void UMusicNote::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	Internal_PlayValidationNote(InDeltaTime);
	Internal_PlayFailNote(InDeltaTime);
}

void UMusicNote::PlayValidationNote(FVector2D NewTargetSize, float NewOpacity)
{
	IsPlayingValidation = true;
	
	NoteValidationFinalScale = NewTargetSize;

	NoteTargetValidationScale = NoteValidationFinalScale * 0.5f;
	NoteImageTargetOpacity = NewOpacity;

	InterpValidationSpeedScale = 20.f; 
	InterpSpeedOpacity = 7.f;
}

void UMusicNote::Internal_PlayValidationNote(float DeltaTime)
{
	if (!IsPlayingValidation || !NoteImageSlot) return;

	NoteCurrentValidationScale = FMath::Vector2DInterpTo(NoteCurrentValidationScale, NoteTargetValidationScale, DeltaTime, InterpValidationSpeedScale);
	NoteImageSlot->SetSize(NoteCurrentValidationScale);

	NoteImageCurrentOpacity = FMath::FInterpTo(NoteImageCurrentOpacity, NoteImageTargetOpacity, DeltaTime, InterpSpeedOpacity);
	NoteImage->SetColorAndOpacity({ GetColorAndOpacity().R, 1, GetColorAndOpacity().B, NoteImageCurrentOpacity });

	if (NoteTargetValidationScale == NoteValidationFinalScale * 0.5f && FVector2D::Distance(NoteCurrentValidationScale, NoteTargetValidationScale) < 1.f)
	{
		NoteTargetValidationScale = NoteValidationFinalScale * 1.3f;
		InterpValidationSpeedScale = 12.f;
	}
	else if (NoteTargetValidationScale == NoteValidationFinalScale * 1.3f && FVector2D::Distance(NoteCurrentValidationScale, NoteTargetValidationScale) < 1.f)
	{
		NoteTargetValidationScale = NoteValidationFinalScale;
		InterpValidationSpeedScale = 8.f;
	}

	// Stop Animation
	bool IsSizeDone = FVector2D::Distance(NoteCurrentValidationScale, NoteValidationFinalScale) < 0.5f;
	bool IsOpacityDone = FMath::Abs(NoteImageCurrentOpacity - NoteImageTargetOpacity) < 0.01f;

	if (IsSizeDone && IsOpacityDone)
	{
		IsPlayingValidation = false;
	}
}


// ---- FAIL NOTE FEEDBACK ----
void UMusicNote::PlayFailNote()
{
	IsPlayingFailAnimation = true;
	NoteTargetRotation = -30.f;
	NoteTargetFailScale = NoteCurrentFailScale * 1.2f;
}

void UMusicNote::Internal_PlayFailNote(float DeltaTime)
{
	if (!IsPlayingFailAnimation || !NoteImageSlot) return;

	NoteCurrentFailScale = FMath::Vector2DInterpTo(NoteCurrentFailScale, NoteTargetFailScale, DeltaTime, InterpFailScaleSpeed);
	NoteCurrentRotation = FMath::FInterpTo(NoteCurrentRotation, NoteTargetRotation, DeltaTime, InterpRotationSpeed);

	NoteImageSlot->SetSize(NoteCurrentFailScale);

	FWidgetTransform ImageTransform = NoteImage->GetRenderTransform();
	ImageTransform.Angle = NoteCurrentRotation;
	NoteImage->SetRenderTransform(ImageTransform);

	const float Tolerance = 0.1f;

	if (FMath::Abs(NoteCurrentRotation - NoteTargetRotation) < Tolerance)
	{
		if (NoteTargetRotation == -30.f)
		{
			NoteTargetRotation = 30.f;
		}
		else if (NoteTargetRotation == 30.f)
		{
			NoteTargetRotation = 1.f;
			NoteTargetFailScale = {50, 50};
		}
		else if (NoteTargetRotation == 1.f && NoteTargetFailScale == NoteCurrentFailScale)
		{
			IsPlayingFailAnimation = false;
		}
	}
}


// ---- IMAGE NOTE ----
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
