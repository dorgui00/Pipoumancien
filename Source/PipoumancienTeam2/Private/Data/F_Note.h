#pragma once

#include "F_Note.generated.h"

class UInputAction;
class USoundCue;

USTRUCT(BlueprintType)
struct F_Note
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Pitch = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UInputAction* InputAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Frequency = 0.8f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* Sound;
};
