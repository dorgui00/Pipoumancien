// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FInvisibleObject.generated.h"

USTRUCT()
struct FInvisibleObject
{
	GENERATED_USTRUCT_BODY()
	
	FInvisibleObject()
	{
		Actor = nullptr;
		Materials = {};
	}
	
	FInvisibleObject(TObjectPtr<AActor> NewActor, const TArray<UMaterialInterface*>& NewMaterials)
	{
		Actor = NewActor;
		Materials = NewMaterials;
	}
	
	bool operator==(FInvisibleObject const& Other) const
	{
		return Actor == Other.Actor;
	}
	
	UPROPERTY()
	TObjectPtr<AActor> Actor;

	UPROPERTY()
	TArray<UMaterialInterface*> Materials;
	
};