// Fill out your copyright notice in the Description page of Project Settings.
#include "Character/PipouCharacter.h"
#include "Character/PipouCharacterStateMachine.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraVisibleTarget.h"
#include "Character/PipouCharacterInputData.h"
#include "Camera/CameraWorldSubsystem.h"
#include "Character/PipouCharacterState.h"
#include "Character/PipouCharacterStateMusic.h"
#include "Components/SphereComponent.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Game/GlobalGameSubsystem.h"
#include "Interaction/Interact.h"
#include "Kismet/GameplayStatics.h"
#include "PNJ/SkeletonController.h"

#pragma region Default Constructors
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

#pragma endregion

#pragma region Default Functions
void APipouCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionCollider->OnComponentBeginOverlap.AddDynamic(this, &APipouCharacter::OnComponentBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.AddDynamic(this, &APipouCharacter::OnComponentEndOverlap);

	CreateStateMachine();
	InitStateMachine();
	
	// Camera
	SetCameraView();
	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);

	if (this->Implements<UCameraVisibleTarget>())
	{
		UE_LOG(LogTemp, Display, TEXT("character added in visible target"));
		GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddVisibleTarget(this);
	}

	// TO EDIT : verif ordre d execution
	GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetCharacters(this);
}

void APipouCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	InteractionCollider->OnComponentBeginOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentBeginOverlap);
	InteractionCollider->OnComponentEndOverlap.RemoveDynamic(this, &APipouCharacter::OnComponentEndOverlap);
}

void APipouCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	TickStateMachine(DeltaTime);
}

#pragma endregion

#pragma region Camera
void APipouCharacter::SetCameraView() const
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController == nullptr) return;
	// TO EDIT use only the camera component not the actor.
	PlayerController->SetViewTargetWithBlend(CameraMain->GetOwner());
}

bool APipouCharacter::IsFollowable()
{
	return true;
}

FVector APipouCharacter::GetFollowPosition()
{
	return GetActorLocation();
}

#pragma endregion

#pragma region Movements&Inputs
void APipouCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Not Needed since the local multiplayer system.
	// SetupMappingContextIntoController();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent == nullptr) return;

	BindInputMoveAndActions(EnhancedInputComponent);
	BindInputMusicActions(EnhancedInputComponent);
}

FVector2D APipouCharacter::GetInputMoveXY() const
{
	return InputMoveXY;
}

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

// Not Needed since the local multiplayer system.
// void APipouCharacter::SetupMappingContextIntoController() const
// {
// 	APlayerController* PlayerController = Cast<APlayerController>(Controller);
// 	if (PlayerController == nullptr) return;
//
// 	ULocalPlayer* Player = PlayerController->GetLocalPlayer();
// 	if (Player == nullptr) return;
//
// 	UEnhancedInputLocalPlayerSubsystem* InputSystem = Player->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
// 	if (InputSystem == nullptr) return;
//
// 	InputSystem->AddMappingContext(InputMappingContext, 0);
// }

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

void APipouCharacter::OnInputMoveXY(const FInputActionValue& InputActionValue)
{
	InputMoveXY = InputActionValue.Get<FVector2D>();
}

#pragma endregion

#pragma region Pipou Class
// Pipou Character State
EPipouCharacterClass APipouCharacter::GetPipouCharacterClass() const
{
	return PipouClass;
}

#pragma endregion

#pragma region Character StateMachine
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

#pragma endregion

#pragma region Music
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

void APipouCharacter::BindInputMusicActions(UEnhancedInputComponent* EnhancedInputComponent)
{
	if (InputData->InputPitch)
	{
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Started, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputPitch);
		EnhancedInputComponent->BindAction(InputData->InputPitch, ETriggerEvent::Completed, this, &APipouCharacter::OnInputPitchCompleted);
	}

	if (InputData->InputNoteA)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteAStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputNoteATriggered);
		EnhancedInputComponent->BindAction(InputData->InputNoteA, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteACompleted);
	}

	if (InputData->InputNoteB)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteBStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputNoteBTriggered);
		EnhancedInputComponent->BindAction(InputData->InputNoteB, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteBCompleted);
	}
	
	if (InputData->InputNoteX)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteXStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Triggered, this, &APipouCharacter::OnInputNoteXTriggered);
		EnhancedInputComponent->BindAction(InputData->InputNoteX, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteXCompleted);
	}

	if (InputData->InputNoteY)
	{
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteYStarted);
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Started, this, &APipouCharacter::OnInputNoteYTriggered);
		EnhancedInputComponent->BindAction(InputData->InputNoteY, ETriggerEvent::Completed, this, &APipouCharacter::OnInputNoteYCompleted);
	}
}


void APipouCharacter::OnInputPitch(const FInputActionValue& InputActionValue)
{
	InputPitch = InputActionValue.Get<float>();
	InputPitchEvent.Broadcast(InputActionValue);
}

void APipouCharacter::OnInputPitchCompleted(const FInputActionValue& InputActionValue)
{
	InputPitchCompleted.Broadcast();
}

void APipouCharacter::OnInputNoteAStarted(const FInputActionValue& InputActionValue)
{
	InputNoteA = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteA);

}

void APipouCharacter::OnInputNoteATriggered(const FInputActionValue& InputActionValue)
{
	InputTriggeredNoteEvent.Broadcast(InputData->InputNoteA);
}

void APipouCharacter::OnInputNoteACompleted(const FInputActionValue& InputActionValue)
{
	InputNoteA = false;
	StateMachine->GetCurrentState()->HasPressedNotes = false;
}

void APipouCharacter::OnInputNoteBStarted(const FInputActionValue& InputActionValue)
{
	InputNoteB = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteB);
}

void APipouCharacter::OnInputNoteBTriggered(const FInputActionValue& InputActionValue)
{
	InputTriggeredNoteEvent.Broadcast(InputData->InputNoteB);
}

void APipouCharacter::OnInputNoteBCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteB = false;
	StateMachine->GetCurrentState()->HasPressedNotes = false;
}

void APipouCharacter::OnInputNoteXStarted(const FInputActionValue& InputActionValue)
{
	InputNoteX = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteX);

}

void APipouCharacter::OnInputNoteXTriggered(const FInputActionValue& InputActionValue)
{
	InputTriggeredNoteEvent.Broadcast(InputData->InputNoteX);
}

void APipouCharacter::OnInputNoteXCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteX = false;
	StateMachine->GetCurrentState()->HasPressedNotes = false;
}

void APipouCharacter::OnInputNoteYStarted(const FInputActionValue& InputActionValue)
{
	InputNoteY = true;
	InputPressedNoteEvent.Broadcast(InputData->InputNoteY);
}

void APipouCharacter::OnInputNoteYTriggered(const FInputActionValue& InputActionValue)
{
	InputTriggeredNoteEvent.Broadcast(InputData->InputNoteY);
}

void APipouCharacter::OnInputNoteYCompleted(const FInputActionValue& InputActionValue)
{
	InputNoteY = false;
	StateMachine->GetCurrentState()->HasPressedNotes = false;
}

#pragma endregion

#pragma region Skeletons Interaction
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
	}
	// Skeleton Interaction
	else if (ASkeletonController* SkeletonController = Cast<ASkeletonController>(OtherActor))
	{
		UGlobalGameSubsystem* GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
		
		// can't interact in transport 
		if (GlobalGameSubsystem->GetWorldState()==EWorldState::WorldTransport) return;

		// overlap to revive or talk with 
		if (SkeletonController->GetState() == ESkeletonState::Dead
			|| SkeletonController->GetState() == ESkeletonState::Dialogue)
		{
			// set current skeleton for myself
			OverlapSkeleton = SkeletonController; // dead or dialogue
		
			// can't retrigger music of a skeleton alive
			if (SkeletonController->GetState() == ESkeletonState::Dead)
			{
				// trying to set current skeleton for everyone
				GlobalGameSubsystem->CheckIfPlayersOverlapSameSkeleton();

				//not everyone is overlapping the same skel
				if (!GlobalGameSubsystem->GetCurrentSkeleton()) return;

				//everyone overlap the same skel
				for (int i = 0; i < 3; ++i)
				{
					GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
					FString::Printf(TEXT("INPUT : %s"), *GlobalGameSubsystem->GetCurrentSkeleton()->MySkeleton->Notes[i].InputAction->GetName()), true, FVector2D(2, 2));
				}
			}
		}
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

				// reset world notes array TO EDIT ?
				// another way to access ResetWorldInteraction() function from CharacterState?
				StateMachine->GetCurrentState()->ResetWorldInteraction(); // CancelWorldInteraction
			}
		}
	}
	// Skeleton Interaction
	else if (Cast<ASkeletonController>(OtherActor))
	{
		// can't interact in transport
		UGlobalGameSubsystem* GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
		if (GlobalGameSubsystem->GetWorldState()==EWorldState::WorldTransport) return;
		
		// delete current skeleton for myself
		OverlapSkeleton = nullptr;
		
		// delete current skeleton for everyone
		GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>()->SetCurrentSkeleton(nullptr);
		UE_LOG(LogTemp, Display, TEXT("End Overlap Skeleton"));
	}
}

#pragma endregion

#pragma region Sounds

void APipouCharacter::InitWorldSoundData()
{
	WorldSoundFromInput =
	{
		{ InputData->InputNoteY, InputSoundData->UpSound},
		{ InputData->InputNoteB, InputSoundData->RightSound},
		{ InputData->InputNoteA, InputSoundData->DownSound}, 
		{ InputData->InputNoteX, InputSoundData->LeftSound}
	};
}

TObjectPtr<USoundBase> APipouCharacter::GetWorldSoundFromInput(TObjectPtr<UInputAction> InputAction) const
{
	return WorldSoundFromInput[InputAction];
}

#pragma endregion



