// Fill out your copyright notice in the Description page of Project Settings.
#include "PipoumancienTeam2/Public/PipouCharacter.h"
#include "PipouCharacterStateMachine.h"

APipouCharacter::APipouCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APipouCharacter::BeginPlay()
{
	Super::BeginPlay();
	CreateStateMachine();
	InitStateMachine();
}

void APipouCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APipouCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

// State Machine
void APipouCharacter::CreateStateMachine()
{
	StateMachine = NewObject<UPipouCharacterStateMachine>(this);
}

void APipouCharacter::InitStateMachine()
{
	if (StateMachine == nullptr) return;
	StateMachine->Init(this);
}
