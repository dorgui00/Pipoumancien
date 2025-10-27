// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DB_Manager.generated.h"

/**
 * 
 */
UCLASS()
class PIPOUMANCIENTEAM2_API UDB_Manager : public UObject
{
	GENERATED_BODY()

private:
	static UDB_Manager* MyInstance;
	
public:
	static UDB_Manager* Instance();
	
	//F_Skeleton* GetSkeletonByID(int ID);
	

};
