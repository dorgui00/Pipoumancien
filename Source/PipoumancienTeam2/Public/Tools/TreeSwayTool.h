// Fill out your copyright notice in the Description page of Project Settings.

// TreeSwayTool.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TreeSwayTool.generated.h"

USTRUCT(BlueprintType)
struct FTreeSwayData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = "Tree")
	AActor* Tree = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Tree")
	FRotator BaseRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, Category = "Tree")
	FRotator TargetOffset = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, Category = "Tree")
	float ElapsedTime = 0.0f;

	UPROPERTY(VisibleAnywhere, Category = "Tree")
	float Delay = 0.0f;

	float Direction = 1.0f;
};

UCLASS()
class PIPOUMANCIENTEAM2_API ATreeSwayTool : public AActor
{
	GENERATED_BODY()

public:
	ATreeSwayTool();

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

#if WITH_EDITOR

	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return bTickInEditor;
	}
#endif

public:

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool")
	FString TreeNamePrefix = TEXT("MOD_Pinetree");

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool")
	bool bScanOnBeginPlay = true;

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool")
	bool bTickInEditor = true;

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway")
	FRotator RotationOffsetMin = FRotator(-5.f, -5.f, -5.f);

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway")
	FRotator RotationOffsetMax = FRotator(5.f, 5.f, 5.f);

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway", meta = (ClampMin = "0.01"))
	float SwayDuration = 2.0f;

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway", meta = (ClampMin = "0.0"))
	float SwayDelayMin = 0.0f;

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway", meta = (ClampMin = "0.0"))
	float SwayDelayMax = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Tree Sway Tool|Sway")
	int32 RandomSeed = 12345;

	UPROPERTY(VisibleAnywhere, Category = "Tree Sway Tool")
	TArray<FTreeSwayData> Trees;

	UFUNCTION(CallInEditor, Category = "Tree Sway Tool")
	void ScanForTrees();

	UFUNCTION(CallInEditor, BlueprintCallable, Category = "Tree Sway Tool")
	void RandomiseSwayTargets();

private:

	FRotator GetRandomOffset() const;
	float GetRandomDelay() const;

	FRandomStream RandomStream;

	FString GetActorIdForMatching(AActor* Actor) const;
};
