#pragma once

#include "F_Skeleton.generated.h"

class USoundCue;
struct F_Note;

USTRUCT(BlueprintType)
struct F_Skeleton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Note> Notes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* BackgroundMusic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Discution;
};
