// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraFollowTarget.h"
#include "GameFramework/Character.h"
#include "UI/PipouHUD.h"
#include "PipouCharacter.generated.h"

class UCameraComponent;
class ASkeletonController;
class IInteract;
class USphereComponent;
struct FInputActionValue;
class UInputMappingContext;
class UAnimMontage;
class UPipouCharacterInputData;
class UPipouCharacterStateMachine;

UENUM()
enum class EPipouCharacterClass : uint8
{
	Necro,
	Phantom,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputPressedNoteEvent, UInputAction*,  InputAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputTriggeredNoteEvent, UInputAction*,  InputAction);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputPitchEvent, FInputActionValue, InputActionValue);
DECLARE_DYNAMIC_DELEGATE(FInputPitchCompleted);

UCLASS()
class PIPOUMANCIENTEAM2_API APipouCharacter : public ACharacter, public ICameraFollowTarget
{
	GENERATED_BODY()

public:
	#pragma region Default Constructor
	APipouCharacter();
	virtual ~APipouCharacter() override;
	
	#pragma endregion

	#pragma region Default Unreal ACharacter Functions
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	#pragma endregion

	#pragma region PipouClass
	// Manage the class Necro or Phantom of the PipouCharacter.
	UPROPERTY(EditDefaultsOnly, Category="Pipou Character")
	EPipouCharacterClass PipouClass;

	EPipouCharacterClass GetPipouCharacterClass() const;
	
	#pragma endregion
	
	#pragma region Character State Machine
	// State Machine
	void CreateStateMachine();
	void InitStateMachine();
	void TickStateMachine(float DeltaTime);

	UPROPERTY()
	TObjectPtr<UPipouCharacterStateMachine> StateMachine;

	#pragma endregion 

	#pragma region Movement&Inputs
	// Return the value of Input Move XY.
	FVector2D GetInputMoveXY() const;

	// Dead Zone to avoid diagonal movement when going forward and backward.
	UPROPERTY(EditAnywhere, Category="Dead Zone")
	float DeadZone = 0.5f;

	// Store the data asset of our input action.
	UPROPERTY()
	TObjectPtr<UPipouCharacterInputData> InputData;
	
	// Not Needed since the local multiplayer system.
	// UPROPERTY()
	// TObjectPtr<UInputMappingContext> InputMappingContext;

	// Variable to get or set the orientation XY of the player mesh.
	FVector2D GetOrientXY() const;
	void SetOrientXY(FVector2D NewOrientXY);

	#pragma endregion 

	#pragma region Camera
	// Store the Camera Main of the game.
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UCameraComponent> CameraMain;
	
	virtual bool IsFollowable() override;
	virtual FVector GetFollowPosition() override;

	#pragma endregion

	#pragma region Music
	// Music
	float GetInputPitch() const;
	bool GetInputNoteA() const;
	bool GetInputNoteB() const;
	bool GetInputNoteX() const;
	bool GetInputNoteY() const;

	UPROPERTY()
	FInputPressedNoteEvent InputPressedNoteEvent;

	UPROPERTY()
	FInputTriggeredNoteEvent InputTriggeredNoteEvent;

	UPROPERTY()
	FInputPitchEvent InputPitchEvent;

	UPROPERTY()
	FInputPitchCompleted InputPitchCompleted;

	#pragma endregion

	#pragma region Interaction
	// Player interaction collider.
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionCollider;

	// Is Overlaping this Interactable Actor.
	UPROPERTY()
	TScriptInterface<IInteract> Interactor;

	#pragma endregion
	
	#pragma region Skeletons
	// Current Skeleton overlap by the interaction collider.
	UPROPERTY()
	ASkeletonController* OverlapSkeleton;
	
	#pragma endregion

protected:
	#pragma region PipouCharacter Default Functions
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	#pragma endregion
	
	#pragma region Camera
	void SetCameraView() const;

	#pragma endregion
	
	#pragma region Movement&Inputs
	// Setup MappingContext for the project.
	// Not Needed since the local multiplayer system.
	// void SetupMappingContextIntoController() const;

	// Variable to store the InputMove XY result.
	UPROPERTY()
	FVector2D InputMoveXY = {0.f, 0.f};

	// Variable and functions for orientation XY.
	UPROPERTY(BlueprintReadOnly)
	FVector2D OrientXY = {1.f, 1.f };
	void RotateMeshUsingOrientXY(float DeltaTime) const;

	#pragma endregion

	#pragma region Music
	// Variable to store the result of each input.
	UPROPERTY()
	bool InputNoteA;

	UPROPERTY()
	bool InputNoteB;

	UPROPERTY()
	bool InputNoteX;

	UPROPERTY()
	bool InputNoteY;

	UPROPERTY()
	float InputPitch = 0;

	#pragma endregion
	
private:
	#pragma region Movement&Inputs
	// Functions to bind and call the event of input for the movement.
	void BindInputMoveAndActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputMoveXY(const FInputActionValue& InputActionValue);

	#pragma endregion
	
	#pragma region Music
	// Functions to call and bind the event of input for the music mechanic.
	void BindInputMusicActions(UEnhancedInputComponent* EnhancedInputComponent);

	void OnInputPitch(const FInputActionValue& InputActionValue);
	void OnInputPitchCompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteAStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteATriggered(const FInputActionValue& InputActionValue);
	void OnInputNoteACompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteBStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteBTriggered(const FInputActionValue& InputActionValue);
	void OnInputNoteBCompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteXStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteXTriggered(const FInputActionValue& InputActionValue);
	void OnInputNoteXCompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteYStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteYTriggered(const FInputActionValue& InputActionValue);
	void OnInputNoteYCompleted(const FInputActionValue& InputActionValue);

	#pragma endregion

	#pragma region Skeletons Interaction Overlap
	// Interact with the Skeletons
	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	#pragma endregion
};
