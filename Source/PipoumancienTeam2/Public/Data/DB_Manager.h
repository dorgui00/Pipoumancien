// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DB_Manager.generated.h"

struct F_Skeleton;
/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API ADB_Manager : public AActor
{
	GENERATED_BODY()

private:
	static ADB_Manager* MyInstance;
	
public:
	static ADB_Manager* Instance(UWorld* World);

	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	UDataTable* DB_Music;
	
	F_Skeleton* GetSkeletonByID(int ID);

};
