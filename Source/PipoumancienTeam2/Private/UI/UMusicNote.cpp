// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UMusicNote.h"

#include "Components/TextBlock.h"

void UMusicNote::SetSlotNote(EMusicNoteType NewNoteType)
{
	SlotNote = NewNoteType;
}

FText UMusicNote::GetNameFromMusicNoteType(EMusicNoteType NoteType)
{
	return SlotTextures[NoteType];
}
