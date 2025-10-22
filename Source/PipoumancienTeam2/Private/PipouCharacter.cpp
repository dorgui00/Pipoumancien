// Fill out your copyright notice in the Description page of Project Settings.
#include "PipoumancienTeam2/Public/PipouCharacter.h"
#include "PipouCharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "PipouCharacterInputData.h"

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
	TickStateMachine(DeltaTime);
}

void APipouCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	SetupMappingContextIntoController();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	BindInputMoveXAxisAndActions(EnhancedInputComponent);
}

// Pipou Character State
EPipouCharacterClass APipouCharacter::GetPipouCharacterClass() const
{
	return PipouClass;
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

void APipouCharacter::TickStateMachine(float DeltaTime)
{
	if (StateMachine == nullptr) return;
	StateMachine->Tick(DeltaTime);
	RotateMeshUsingOrientXY(DeltaTime);
}

void APipouCharacter::SetupMappingContextIntoController() const
{
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController == nullptr) return;

	ULocalPlayer* Player = PlayerController->GetLocalPlayer();
	if (Player == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSystem == nullptr) return;

	InputSystem->AddMappingContext(InputMappingContext, 0);
}

// Move
FVector2D APipouCharacter::GetInputMoveXY() const
{
	return InputMoveXY;
}

// Orient
void APipouCharacter::RotateMeshUsingOrientXY(float DeltaTime) const
{
	if (OrientXY.IsNearlyZero())
		return;

	float AngleRadians = FMath::Atan2(OrientXY.Y, OrientXY.X);
	float TargetYaw = FMath::RadiansToDegrees(AngleRadians) - 90.f;  

	FRotator CurrentRotation = GetMesh()->GetRelativeRotation();
	FRotator TargetRotation = FRotator(CurrentRotation.Pitch, TargetYaw, CurrentRotation.Roll);
	FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 10.f);
	GetMesh()->SetRelativeRotation(NewRotation);
}

FVector2D APipouCharacter::GetOrientXY() const
{
	return OrientXY;
}

void APipouCharacter::SetOrientXY(FVector2D NewOrientXY)
{
	OrientXY = NewOrientXY;
}

// Music
float APipouCharacter::GetInputPitch() const
{
	return InputPitch;
}

bool APipouCharacter::GetInputNoteA() const
{
	return InputNoteA;
}

bool APipouCharacter::GetInputNoteB() const
{
	return InputNoteB;
}

bool APipouCharacter::GetInputNoteX() const
{
	return InputNoteX;
}

bool APipouCharacter::GetInputNoteY() const
{
	return InputNoteY; 
}


void APipouCharacter::BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;

	if (InputData->InputActionMoveXY)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Started, this, &APipouCharacter::OnInputMoveXY);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputMoveXY);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Completed, this, &APipouCharacter::OnInputMoveXY);
	}

	if (InputData->InputPitch)
	{
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Started, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Completed, this, &APipouCharacter::OnInputPitch);
	}

	if (InputData->InputNoteA)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteA);
	}

	if (InputData->InputNoteB)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteB);
	}
	
	if (InputData->InputNoteX)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteX);
	}

	if (InputData->InputNoteY)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteY);
	}
}

void APipouCharacter::OnInputMoveXY(const FInputActionValue& InputActionValue)
{
	InputMoveXY = InputActionValue.Get<FVector2D>();
}

void APipouCharacter::OnInputPitch(const FInputActionValue& InputActionValue)
{
	InputPitch = InputActionValue.Get<float>();
}

void APipouCharacter::OnInputNoteA(const FInputActionValue& InputActionValue)
{
	InputNoteA = InputActionValue.Get<bool>();
}

void APipouCharacter::OnInputNoteB(const FInputActionValue& InputActionValue)
{
	InputNoteB = InputActionValue.Get<bool>();
}

void APipouCharacter::OnInputNoteX(const FInputActionValue& InputActionValue)
{
	InputNoteX = InputActionValue.Get<bool>();
}

void APipouCharacter::OnInputNoteY(const FInputActionValue& InputActionValue)
{
	InputNoteY = InputActionValue.Get<bool>();
}



