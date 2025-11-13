// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/F_Skeleton.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GlobalDataTableSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalDataTableSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private :	
	UPROPERTY()
	UDataTable* DT_Music;

public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	F_Skeleton* GetSkeletonByID(int ID) const;
};
