// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraFollowTarget.h"
#include "GameFramework/Character.h"
#include "UI/PipouHUD.h"
#include "PipouCharacter.generated.h"

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInputPitchEvent, FInputActionValue, InputActionValue);

UCLASS()
class PIPOUMANCIENTEAM2_API APipouCharacter : public ACharacter, public ICameraFollowTarget
{
	GENERATED_BODY()

public:
	APipouCharacter();
	virtual ~APipouCharacter() override;

	// Dead Zone
	UPROPERTY(EditAnywhere, Category="Dead Zone")
	float DeadZone = 0.5f;

	// Pipou State
	UPROPERTY(EditDefaultsOnly, Category="Pipou Character")
	EPipouCharacterClass PipouClass;

	EPipouCharacterClass GetPipouCharacterClass() const;

	// State Machine
	void CreateStateMachine();
	void InitStateMachine();
	void TickStateMachine(float DeltaTime);

	UPROPERTY()
	TObjectPtr<UPipouCharacterStateMachine> StateMachine;
	
	// Inputs
	UPROPERTY()
	TObjectPtr<UPipouCharacterInputData> InputData;
	
	UPROPERTY()
	TObjectPtr<UInputMappingContext> InputMappingContext;

	// Move
	FVector2D GetInputMoveXY() const;

	// Camera
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<AActor> CameraActor;

	virtual bool IsFollowable() override;
	virtual FVector GetFollowPosition() override;

	// Orient
	FVector2D GetOrientXY() const;
	void SetOrientXY(FVector2D NewOrientXY);

	// Music
	float GetInputPitch() const;
	bool GetInputNoteA() const;
	bool GetInputNoteB() const;
	bool GetInputNoteX() const;
	bool GetInputNoteY() const;

	UPROPERTY()
	FInputPressedNoteEvent InputPressedNoteEvent;

	UPROPERTY()
	FInputPitchEvent InputPitchEvent;
	
	// Collider
	UPROPERTY(VisibleAnywhere)
	USphereComponent* InteractionCollider;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// Camera
	void SetCameraView() const;

	// Inputs
	void SetupMappingContextIntoController() const;

	// Move
	UPROPERTY()
	FVector2D InputMoveXY = {0.f, 0.f};

	// Orient
	UPROPERTY(BlueprintReadOnly)
	FVector2D OrientXY = {1.f, 1.f };
	void RotateMeshUsingOrientXY(float DeltaTime) const;

	// Music
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
	
public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// Interact 
	UPROPERTY()
	TScriptInterface<IInteract> Interactor ; // Is Overlaping this Interactable Actor

	// CurrentSkeletonOverlap
	ASkeletonController* OverlapSkeleton;

private:
	// Move
	void BindInputMoveAndActions(UEnhancedInputComponent* EnhancedInputComponent);
	void BindInputMusicActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputMoveXY(const FInputActionValue& InputActionValue);

	// Music
	void OnInputPitch(const FInputActionValue& InputActionValue);
	void OnInputNoteAStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteACompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteBStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteBCompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteXStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteXCompleted(const FInputActionValue& InputActionValue);
	void OnInputNoteYStarted(const FInputActionValue& InputActionValue);
	void OnInputNoteYCompleted(const FInputActionValue& InputActionValue);

	UFUNCTION()
	void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};
