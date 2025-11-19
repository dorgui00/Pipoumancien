#pragma once
#include "F_Dialoge.generated.h"

USTRUCT(BlueprintType)
struct F_Dialoge : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int ID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Discution;
};
