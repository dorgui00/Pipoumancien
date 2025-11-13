// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraCharacter.h"

#include "Camera/CameraComponent.h"
#include "Camera/CameraWorldSubsystem.h"


// Sets default values
ACameraCharacter::ACameraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ACameraCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetSubsystem<UCameraWorldSubsystem>()->AddFollowTarget(this);
}

// Called every frame
void ACameraCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ACameraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool ACameraCharacter::IsFollowable()
{
	return true;
}

FVector ACameraCharacter::GetFollowPosition()
{
	return GetActorLocation();
}

