// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/USlot.h"

#include "Components/TextBlock.h"

void USlot::SetSlotNote(EMusicNoteType NewNoteType)
{
	SlotNote = NewNoteType;
	LetterText->SetText(GetNameFromMusicNoteType(NewNoteType));
	LetterText->SetColorAndOpacity(FSlateColor(FLinearColor({0, 0, 0, 1.f})));
}

FText USlot::GetNameFromMusicNoteType(EMusicNoteType NoteType)
{
	return SlotTextures[NoteType];
}
