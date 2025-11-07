// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DataTableGameInstanceSubsystem.h"

void UDataTableGameInstanceSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// // First try
	// if (DT_MusicAsset.IsValid())
	// {
	// 	DT_Music = DT_MusicAsset.Get();
	// }
	// // Second try async by path
	// else if (DT_MusicAsset.ToSoftObjectPath().IsValid())
	// {
	// 	DT_Music = DT_MusicAsset.LoadSynchronous();
	// }

	// TO EDIT
	DT_Music = LoadObject<UDataTable>(nullptr, TEXT("/Game/Pipoumancien/Datas/DT_Music.DT_Music"));

	if (!DT_Music)
		UE_LOG(LogTemp, Error, TEXT("DT_Music loading failed."));
}

F_Skeleton* UDataTableGameInstanceSubsystem::GetSkeletonByID(int ID) const
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
