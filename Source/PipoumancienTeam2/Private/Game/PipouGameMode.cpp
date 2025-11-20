// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/PipouGameMode.h"

#include "LocalMultiplayerSettings.h"
#include "LocalMultiplayerSubsystem.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacter.h"
#include "Editor/MapSettings.h"
#include "Editor/PipouCharacterSettings.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"

class AGameManager;

void APipouGameMode::BeginPlay()
{
	Super::BeginPlay();

	CreateAndInitPlayers();
	FindPlayerStartActorsInScene(PlayerStartsPoint);
	
	
	// Init the camera main in the CameraWorldSubsystem.
	if (UCameraWorldSubsystem* CameraWorldSubsystem = GetWorld()->GetSubsystem<UCameraWorldSubsystem>())
		CameraWorldSubsystem->OnCamerasReady.AddDynamic(this, &APipouGameMode::OnCamerasReady);
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

void APipouGameMode::OnCamerasReady()
{
	// Init the camera main variable in this script with the one in the CameraWorldSubsystem.
	GetCamera();
	
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
	UPipouCharacterInputData* InputData = LoadInputDataFromConfig();
	// UInputMappingContext* InputMappingContext = LoadInputMappingContextFromConfig();
	
	for (APlayerStart* SpawnPoint : SpawnPoints)
	{
		EAutoReceiveInput::Type InputType = SpawnPoint->AutoReceiveInput.GetValue();
		TSubclassOf<APipouCharacter> PipouCharacterClass = GetPipouCharacterFromInputType(InputType);
		if (PipouCharacterClass == nullptr) continue;

		APipouCharacter* NewCharacter = GetWorld()->SpawnActorDeferred<APipouCharacter>(PipouCharacterClass, SpawnPoint->GetTransform());
		if (NewCharacter == nullptr) continue;
		
		NewCharacter->CameraMain = CameraMain;
		NewCharacter->InputData =  InputData;
		// NewCharacter->InputMappingContext = InputMappingContext;
		NewCharacter->SetOrientXY(FVector2D(CameraMain->GetForwardVector().X, CameraMain->GetForwardVector().Y));
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

void APipouGameMode::GetCamera()
{
	TObjectPtr<UCameraWorldSubsystem> CameraSubsystem = GetWorld()->GetSubsystem<UCameraWorldSubsystem>();
	if (!CameraSubsystem) return;
	CameraMain = CameraSubsystem->CameraMain;
}

void APipouGameMode::CreateAndInitPlayers()
{
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	if (GameInstance == nullptr) return;

	ULocalMultiplayerSubsystem* LocalMultiplayerSubsystem = GameInstance->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (LocalMultiplayerSubsystem == nullptr) return;

	LocalMultiplayerSubsystem->CreateAndInitPlayers(ELocalMultiplayerInputMappingType::InGame);
}
