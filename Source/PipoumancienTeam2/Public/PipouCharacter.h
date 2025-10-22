// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PipouCharacter.generated.h"

struct FInputActionValue;
class UInputMappingContext;

class UAnimMontage;
class UPipouCharacterInputData;
class UPipouCharacterStateMachine;

UENUM()
enum class EPipouCharacterClass : uint8
{
	Necromancer,
	Phantom,
};

UCLASS()
class PIPOUMANCIENTEAM2_API APipouCharacter : public ACharacter
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
	UPROPERTY()
	TObjectPtr<AActor> CameraActor;

	// Orient
	FVector2D GetOrientXY() const;
	void SetOrientXY(FVector2D NewOrientXY);

	// Music
	float GetInputPitch() const;
	bool GetInputNoteA() const;
	bool GetInputNoteB() const;
	bool GetInputNoteX() const;
	bool GetInputNoteY() const;

protected:
	virtual void BeginPlay() override;
	
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
	void BindInputMoveXAxisAndActions(UEnhancedInputComponent* EnhancedInputComponent);
	void OnInputMoveXY(const FInputActionValue& InputActionValue);

	// Music
	void OnInputPitch(const FInputActionValue& InputActionValue);
	void OnInputNoteA(const FInputActionValue& InputActionValue);
	void OnInputNoteB(const FInputActionValue& InputActionValue);
	void OnInputNoteX(const FInputActionValue& InputActionValue);
	void OnInputNoteY(const FInputActionValue& InputActionValue);
	
};
