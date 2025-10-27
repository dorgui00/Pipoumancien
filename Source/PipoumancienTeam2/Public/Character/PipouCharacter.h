// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Camera/CameraFollowTarget.h"
#include "GameFramework/Character.h"
#include "PipouCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UAnimMontage;
class UPipouCharacterInputData;
class UPipouCharacterStateMachine;
class ICameraFollowTarget;

UENUM()
enum class EPipouCharacterClass : uint8
{
	Necro,
	Phantom,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInputPressedEvent, UInputAction*,  InputAction, FInputActionValue, InputActionValue);

UCLASS()
class PIPOUMANCIENTEAM2_API APipouCharacter : public ACharacter, public ICameraFollowTarget
{
	GENERATED_BODY()

public:
	APipouCharacter();

	// Dead Zone
	UPROPERTY(EditAnywhere, Category="Dead Zone")
	float DeadZone = 0.5f;

	// Pipou State
	UPROPERTY(EditAnywhere)
	EPipouCharacterClass PipouClass;

	EPipouCharacterClass GetPipouCharacterClass() const;
		
	// State Machine
	void CreateStateMachine();
	void InitStateMachine();
	void TickStateMachine(float DeltaTime);
	
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
	FInputPressedEvent InputPressedEvent;

protected:
	virtual void BeginPlay() override;

	// Camera
	void SetCameraView() const;
	
	// State Machine
	UPROPERTY()
	TObjectPtr<UPipouCharacterStateMachine> StateMachine;

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

	// Music
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	

};
