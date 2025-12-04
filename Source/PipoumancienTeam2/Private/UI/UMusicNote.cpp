// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMusicNote.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/StructuredLog.h"
#include "UI/GlobalHUDSubsystem.h"

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
