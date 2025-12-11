#pragma once
#include "Data/FSkeletonVisuals.h"
#include "Data/F_Note.h"
#include "F_Skeleton.generated.h"

class USoundCue;

USTRUCT(BlueprintType)
struct F_Skeleton : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	// MUSIC
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Note> Notes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MaxFailNotePossible = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* BackgroundMusic;

	// DIALOGUE
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Discution;

	// SPRITE / ANIMS 
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSkeletonVisuals SkeletonVisuals;

};
