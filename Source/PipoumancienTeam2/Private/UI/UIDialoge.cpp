// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIDialoge.h"
#include "UResurrectionWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Data/F_Skeleton.h"



void UUIDialoge::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentCharIndex = 0;
	bIsFocusable = true;
	Name->SetText(FText::FromString(CurrentName));
	// Donne le focus clavier au widget
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		PC->bShowMouseCursor = true;
	}

	// On ne lit PLUS la DataTable ici → c’est SetDialogue() qui charge le texte

	if (CurrentDialogue.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ Aucun dialogue trouvé !"));
		return;
	}

	// Lancer l’affichage lettre par lettre
	GetWorld()->GetTimerManager().SetTimer(
		TextTimerHandle,
		this,
		&UUIDialoge::ShowNextCharacter,
		TextSpeed,
		true
	);
}


void UUIDialoge::SetDialogue(F_Skeleton* Skeleton)
{
	CurrentDialogue = Skeleton->Discution;
	CurrentName = Skeleton->Name.ToString();

	CurrentDialogueIndex = 0;
	CurrentCharIndex = 0;

	if (CurrentDialogue.Num() > 0)
		FullText = CurrentDialogue[0];

	AddToViewport();
}

FReply UUIDialoge::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::E)
	{
		// Passer à la phrase suivante
		CurrentDialogueIndex++;

		// Plus de phrases → fin
		if (CurrentDialogueIndex >= CurrentDialogue.Num())
		{
			RemoveFromParent();
			UE_LOG(LogTemp, Warning, TEXT("✔ Fin du dialogue"));
			return FReply::Handled();
		}

		// Charger la nouvelle phrase
		FullText = CurrentDialogue[CurrentDialogueIndex];
		CurrentCharIndex = 0;

		// Redémarrer l’affichage des lettres
		GetWorld()->GetTimerManager().ClearTimer(TextTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(
			TextTimerHandle,
			this,
			&UUIDialoge::ShowNextCharacter,
			TextSpeed,
			true
		);

		return FReply::Handled();
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UUIDialoge::ShowNextCharacter()
{
	if (!Discution) return;

	if (CurrentCharIndex < FullText.Len())
	{
		FString CurrentText = FullText.Left(CurrentCharIndex + 1);
		Discution->SetText(FText::FromString(CurrentText));
		CurrentCharIndex++;
	}
	else
	{
		// Fin de la phrase : stop le timer
		GetWorld()->GetTimerManager().ClearTimer(TextTimerHandle);
	}
}
