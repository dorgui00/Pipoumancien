// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Character/PipouCharacterInputData.h"
#include "Camera/CameraWorldSubsystem.h"

#include "Components/SphereComponent.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Interaction/Interact.h"
#include "PNJ/SkeletonController.h"

APipouCharacter::APipouCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InteractionCollider = CreateDefaultSubobject<USphereComponent>(TEXT("InteractColl"));
	InteractionCollider->InitSphereRadius(100.0f);
	InteractionCollider->SetupAttachment(GetRootComponent());
}

APipouCharacter::~APipouCharacter()
{
	// if (InteractionCollider)
	// {
	// 	InteractionCollider->OnComponentBeginOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentBeginOverlap);
	// 	InteractionCollider->OnComponentEndOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentEndOverlap);
	// }
}


void APipouCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &APipouCharacter::OnComponentBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &APipouCharacter::OnComponentEndOverlap);

	InitPipouHUD();
	CreateStateMachine();
	InitStateMachine();
	SetCameraView();
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);

	// TO EDIT : verif ordre d execution
	GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetCharacters(this);
}

void APipouCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	InteractionCollider->OnComponentBeginOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentEndOverlap);
}

void APipouCharacter::SetCameraView() const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr) return;
	PlayerController->SetViewTargetWithBlend(CameraActor);
}

// UI
void APipouCharacter::InitPipouHUD()
{
	PipouHUD = Cast<APipouHUD>(GetController<APlayerController>()->GetHUD());
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

	BindInputMoveAndActions(EnhancedInputComponent);
	BindInputMusicActions(EnhancedInputComponent);
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

// Camera
bool APipouCharacter::IsFollowable()
{
	return true;
}

FVector APipouCharacter::GetFollowPosition()
{
	return GetActorLocation();
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

void APipouCharacter::BindInputMoveAndActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData == nullptr) return;

	if (InputData->InputActionMoveXY)
	{
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Started, this, &APipouCharacter::OnInputMoveXY);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputMoveXY);
		EnhancedInputComponent->BindAction(InputData->InputActionMoveXY, ETriggerEvent::Completed, this, &APipouCharacter::OnInputMoveXY);
	}
}

void APipouCharacter::BindInputMusicActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData->InputPitch)
	{
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Started, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Completed, this, &APipouCharacter::OnInputPitch);
	}

	if (InputData->InputNoteA)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteAStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteACompleted);
	}

	if (InputData->InputNoteB)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteBStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteBCompleted);
	}
	
	if (InputData->InputNoteX)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteXStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteXCompleted);
	}

	if (InputData->InputNoteY)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteYStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteYCompleted);
	}
}

void APipouCharacter::OnInputMoveXY(const FInputActionValue& InputActionValue)
{
	InputMoveXY = InputActionValue.Get<FVector2D>();
}

void APipouCharacter::OnInputPitch(const FInputActionValue& InputActionValue)
{
	InputPitch = InputActionValue.Get<float>();
	InputPitchEvent.Broadcast(InputActionValue);
}

void APipouCharacter::OnInputNoteAStarted(const FInputActionValue& InputActionValue)
{
	InputNoteA = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteA);

	// play son A
}

void APipouCharacter::OnInputNoteACompleted(const FInputActionValue& InputActionValue)
{
	InputNoteA = false;
}

void APipouCharacter::OnInputNoteBStarted(const FInputActionValue& InputActionValue)
{
	InputNoteB = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteB);

	// play son B
}

void APipouCharacter::OnInputNoteBCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteB = false;
}

void APipouCharacter::OnInputNoteXStarted(const FInputActionValue& InputActionValue)
{
	InputNoteX = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteX);

	// play son x
}

void APipouCharacter::OnInputNoteXCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteX = false;
}

void APipouCharacter::OnInputNoteYStarted(const FInputActionValue& InputActionValue)
{
	InputNoteY = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteY);

	// play son Y
}

void APipouCharacter::OnInputNoteYCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteY = false;
}

void APipouCharacter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	UE_LOG(LogTemp, Display, TEXT("Begin Overlap"));

	// World Interaction
	//if interactable
	if (OtherActor->Implements<UInteract>())
	{
		if (IInteract* InInteractor = Cast<IInteract>(OtherActor))
		{
				Interactor.SetInterface(InInteractor); // update current interactor
				Interactor.SetObject(OtherActor);

				//IInteract::Execute_Interact(Interactor.GetObject()); 
		}
	};

	// Skeleton Interaction
	ASkeletonController* SkeletonController = Cast<ASkeletonController>(OtherActor);

	if (SkeletonController)
	{
		UGlobalGameSubsystem* GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
		GlobalGameSubsystem->SetCurrentSkeleton(SkeletonController->MySkeleton);

		for (int i = 0; i < 3; ++i)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
			FString::Printf(TEXT("INPUT : %s"), *GlobalGameSubsystem->GetCurrentSkeleton()->Notes[i].InputAction->GetName()), true, FVector2D(2, 2));
		}
		
		
		UE_LOG(LogTemp, Display, TEXT("Begin Overlap Skeleton"));
	}
}

void APipouCharacter::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Display, TEXT("End Overlap"));

	// World Interaction
	//if interactable
	if (OtherActor->Implements<UInteract>())
	{
		if (IInteract* InInteractor = Cast<IInteract>(OtherActor))
		{
			if (InInteractor==Interactor.GetInterface())
			{
				Interactor.SetInterface(nullptr); // set current interactor to null
				Interactor.SetObject(nullptr);
			}
		}
	}
	
	// Skeleton Interaction
	if (ASkeletonController* SkeletonController = Cast<ASkeletonController>(OtherActor))
	{
		GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetCurrentSkeleton(nullptr);
		UE_LOG(LogTemp, Display, TEXT("End Overlap Skeleton"));
	}
}




