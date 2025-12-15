// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/UIDialoge.h"
#include "UResurrectionWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UUIDialoge::NativeConstruct()
{
	Super::NativeConstruct();
	CurrentCharIndex = 0;
	bIsFocusable = true;

	if (Name)
		Name->SetText(FText::FromString(CurrentName));
	
	// Donne le focus clavier au widget
	if (APlayerController* PC = GetOwningPlayer())
	{
		FInputModeUIOnly InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
		//PC->bShowMouseCursor = true;
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


void UUIDialoge::SetDialogue(F_Skeleton* Skeleton , int Valut)
{
	if (!IsInViewport())
	{
		CurrentSkeleton = Skeleton ;
		CurrentDialogue = Skeleton->Discution;
		CurrentName = Skeleton->Name.ToString();

		CurrentDialogueIndex = 0;
		CurrentCharIndex = 0;
		if (Valut == 1)
		{
			CurrentDialogueIndex = 1;
		}
	

		if (CurrentDialogue.Num() > 0 && (CurrentDialogueIndex <= CurrentDialogue.Num()-1))
			FullText = CurrentDialogue[CurrentDialogueIndex];

		AddToViewport();
	}
}

void UUIDialoge::GoToNextDialogue()
{
	// Passer à la phrase suivante
	CurrentDialogueIndex++;

	// Plus de phrases → fin
	if (CurrentDialogueIndex >= CurrentDialogue.Num() - 1)
	{
		RemoveFromParent();
		UE_LOG(LogTemp, Warning, TEXT("✔ Fin du dialogue"));

		// fin du dialogue state => retour au monde normal
		GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetWorldFreeState();

		return ; 
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

			// fin du dialogue state => retour au monde normal
			GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetWorldFreeState();
			
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

		// --- TO EDIT ---
		if (CurrentSkeleton->Voices.Num() != 0)
			UGameplayStatics::PlaySound2D(GetWorld(),CurrentSkeleton->Voices[0]);
	}
	else
	{
		// Fin de la phrase : stop le timer
		GetWorld()->GetTimerManager().ClearTimer(TextTimerHandle);
	}
}
