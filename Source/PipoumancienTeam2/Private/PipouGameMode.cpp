// Fill out your copyright notice in the Description page of Project Settings.


#include "PipoumancienTeam2/Public/PipouGameMode.h"
#include "PipouCharacter.h"
#include "PlayerSettings.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void APipouGameMode::BeginPlay()
{
	Super::BeginPlay();

	TArray<APlayerStart*> PlayerStartsPoint;
	FindPlayerStartActorsInScene(PlayerStartsPoint);
	SpawnCharacters(PlayerStartsPoint);
}

void APipouGameMode::FindPlayerStartActorsInScene(TArray<APlayerStart*>& ResultActors)
{
	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(),PlayerStartActors);

	for (int i = 0; i < PlayerStartActors.Num(); ++i)
	{
		APlayerStart* PlayerStartActor = Cast<APlayerStart>(PlayerStartActors[i]);
		if (PlayerStartActor == nullptr) continue;

		ResultActors.Add(PlayerStartActor);
	}
}

void APipouGameMode::SpawnCharacters(const TArray<APlayerStart*>& SpawnPoints)
{
	for (APlayerStart* SpawnPoint : SpawnPoints)
	{
		EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
		TSubclassOf<APipouCharacter> PipouCharacterClass = GetPipouCharacterFromInputType(InputType);
		if (PipouCharacterClass == nullptr) continue;

		APipouCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<APipouCharacter>(PipouCharacterClass, SpawnPoint->GetTransform());

		if (NewCharacter == nullptr) continue;
		NewCharacter->AutoPossessPlayer = SpawnPoint->AutoReceiveInput;
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

		CharactersInMap.Add(NewCharacter);
	}
}

TSubclassOf<APipouCharacter> APipouGameMode::GetPipouCharacterFromInputType(EAutoReceiveInput::Type InputType) const
{
	const UPlayerSettings* PlayerSettings = GetDefault<UPlayerSettings>();
	
	switch (InputType)
	{
	case EAutoReceiveInput::Player0:
		return PlayerSettings->PipouCharacterClassP0;
	case EAutoReceiveInput::Player1:
		return PlayerSettings->PipouCharacterClassP1;
	default:
		return nullptr;
	}
}
