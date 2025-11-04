// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/F_Skeleton.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DataTableGameInstanceSubsystem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class PIPOUMANCIENTEAM2_API UDataTableGameInstanceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private :
	UPROPERTY(EditDefaultsOnly,Category="Data")
	TSoftObjectPtr<UDataTable> DT_MusicAsset;
	
	UPROPERTY()
	TObjectPtr<UDataTable> DT_Music;

public :
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	F_Skeleton* GetSkeletonByID(int ID) const;
};
