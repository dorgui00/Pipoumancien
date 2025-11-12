// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/GlobalDataTableSubsystem.h"

#include "Settings/SubsystemSettings.h"

void UGlobalDataTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	
	if (SubsystemSettings)
		DT_Music = SubsystemSettings->DT_Music.LoadSynchronous();

	if (!DT_Music)
		UE_LOG(LogTemp, Error, TEXT("DT_Music loading failed."));
}

F_Skeleton* UGlobalDataTableSubsystem::GetSkeletonByID(int ID) const
{
	TArray<F_Skeleton*> Skeletons;
	
	if (DT_Music)
	{
		DT_Music->GetAllRows("", Skeletons);
	
		for (const auto Skeleton : Skeletons)
		{
			if (Skeleton->ID == ID)
			{
				return Skeleton;
			}
		}
	}
	
	return nullptr;
}
