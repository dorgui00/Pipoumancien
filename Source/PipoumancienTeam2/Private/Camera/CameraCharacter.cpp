// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/CameraCharacter.h"
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

	// FVector NewPosition = GetActorLocation();
	//
	// UCameraWorldSubsystem* prout = GetWorld()->GetSubsystem<UCameraWorldSubsystem>();
 //    if(prout == nullptr) return;
 //    
	// FVector2D OutViewportBoundsMin = FVector2D::ZeroVector;
	// FVector2D OutViewportBoundsMax = FVector2D::ZeroVector;
	// prout->GetViewportBounds(OutViewportBoundsMin,OutViewportBoundsMax);
 //
	// FVector min = FVector(OutViewportBoundsMin.X,OutViewportBoundsMin.Y,NewPosition.Z);
	// FVector max = FVector(OutViewportBoundsMax.X,OutViewportBoundsMax.Y,NewPosition.Z);
	// //NewPosition = FMath::Clamp(NewPosition,min,max);
 //
	// NewPosition.X = FMath::Clamp(NewPosition.X, min.X, max.X);
	// NewPosition.Y = FMath::Clamp(NewPosition.Y, min.Y, max.Y);
	//
	// this->SetActorLocation(NewPosition);
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

