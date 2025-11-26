// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/PipouCharacterStateDialogue.h"

#include "Character/PipouCharacter.h"
#include "PNJ/SkeletonController.h"
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
	// Pass to next dialogue
	if (Character->OverlapSkeleton && Character->OverlapSkeleton->PlayerWidget)
	{
		Character->OverlapSkeleton->PlayerWidget->GoToNextDialogue();
	}
	
}
