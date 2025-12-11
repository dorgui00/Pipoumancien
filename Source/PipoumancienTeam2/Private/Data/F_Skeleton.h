#pragma once
#include "Data/FSkeletonVisuals.h"
#include "F_Skeleton.generated.h"

struct FSkeletonVisuals;
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

	// MUSIC
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<F_Note> Notes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* BackgroundMusic;

	// DIALOGUE
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Discution;

	// SPRITE / ANIMS 
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	FSkeletonVisuals SkeletonVisuals;

};
