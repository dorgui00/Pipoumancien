// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateDialogue.h"

#include "Character/PipouCharacter.h"
#include "Enums/EInputAction.h"
#include "PNJ/SkeletonController.h"
#include "Tools/Slider/NoteMapping.h"
#include "UI/UIDialoge.h"


EPipouCharacterStateID UPipouCharacterStateDialogue::GetStateID()
{
	return EPipouCharacterStateID::Dialogue;
}

void UPipouCharacterStateDialogue::StateEnter(EPipouCharacterStateID PreviousStateID)
{
	Super::StateEnter(PreviousStateID);
	
	Character->InputPressedNoteEvent.AddDynamic(this, &UPipouCharacterStateDialogue::OnCharacterPressedNote);
}

void UPipouCharacterStateDialogue::StateTick(float Deltatime)
{
	Super::StateTick(Deltatime);
	
}

void UPipouCharacterStateDialogue::StateExit(EPipouCharacterStateID NextStateID)
{
	Super::StateExit(NextStateID);
	
	Character->InputPressedNoteEvent.RemoveDynamic(this, &UPipouCharacterStateDialogue::OnCharacterPressedNote);
}

void UPipouCharacterStateDialogue::OnCharacterPressedNote(UInputAction* InputAction)
{
	// Interact with A
	if (InputAction != Character->InputData->InputNoteA) return;
	
	// Pass to next dialogue
	if (Character->OverlapSkeleton && Character->OverlapSkeleton->PlayerWidget)
	{
		Character->OverlapSkeleton->PlayerWidget->GoToNextDialogue();
	}
	
}
