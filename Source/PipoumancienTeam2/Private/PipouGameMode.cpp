// Fill out your copyright notice in the Description page of Project Settings.


#include "PipoumancienTeam2/Public/PipouGameMode.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "PipouCharacter.h"
#include "MapSettings.h"
#include "PipouCharacterSettings.h"
#include "Camera/CameraActor.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void APipouGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateAndInitPlayers();
	TArray<APlayerStart*> PlayerStartsPoint;
	FindPlayerStartActorsInScene(PlayerStartsPoint);
	GetCameraByTag("PipouCamera");
	SpawnCharacters(PlayerStartsPoint);
}

UPipouCharacterInputData* APipouGameMode::LoadInputDataFromConfig()
{
	const UPipouCharacterSettings* CharacterSettings = GetDefault<UPipouCharacterSettings>();
	if (CharacterSettings == nullptr) return nullptr;
	return CharacterSettings->InputData.LoadSynchronous();
}

UInputMappingContext* APipouGameMode::LoadInputMappingContextFromConfig()
{
	const UPipouCharacterSettings* CharacterSettings = GetDefault<UPipouCharacterSettings>();
	if (CharacterSettings == nullptr) return nullptr;
	return CharacterSettings->InputMappingContext.LoadSynchronous();
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
	UPipouCharacterInputData* InputData = LoadInputDataFromConfig();
	// UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();
	
	for (APlayerStart* SpawnPoint : SpawnPoints)
	{
		EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
		TSubclassOf<APipouCharacter> PipouCharacterClass = GetPipouCharacterFromInputType(InputType);
		if (PipouCharacterClass == nullptr) continue;

		APipouCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<APipouCharacter>(PipouCharacterClass, SpawnPoint->GetTransform());

		if (NewCharacter == nullptr) continue;
		NewCharacter->InputData =  InputData;
		NewCharacter->CameraActor = CameraActor;
		// NewCharacter->InputMappingContext = InputMappingContext;
		NewCharacter->SetOrientXY(FVector2D(CameraActor->GetActorForwardVector().X, CameraActor->GetActorForwardVector().Y));
		NewCharacter->AutoPossessPlayer = InputType;
		
		NewCharacter->FinishSpawning(SpawnPoint->GetTransform());

		CharactersInMap.Add(NewCharacter);
	}
}

TSubclassOf<APipouCharacter> APipouGameMode::GetPipouCharacterFromInputType(EAutoReceiveInput::Type InputType) const
{
	const UMapSettings* PlayerSettings = GetDefault<UMapSettings>();
	
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

void APipouGameMode::GetCameraByTag(const FName& CameraTag)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), CameraTag, FoundActors);
	CameraActor = Cast<ACameraActor>(FoundActors[0]);
}

void APipouGameMode::CreateAndInitPlayers()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance == nullptr) return;

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}
