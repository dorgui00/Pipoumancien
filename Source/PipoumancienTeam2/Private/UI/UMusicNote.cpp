// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMusicNote.h"

#include "Components/TextBlock.h"

void UMusicNote::SetSlotNote(EMusicNoteType NewNoteType)
{
	SlotNote = NewNoteType;
	LetterText->SetText(GetNameFromMusicNoteType(NewNoteType));
	LetterText->SetColorAndOpacity(FSlateColor(FLinearColor({1.f, 1.f, 1.f, 1.f})));
}

FText UMusicNote::GetNameFromMusicNoteType(EMusicNoteType NoteType)
{
	return SlotTextures[NoteType];
}
