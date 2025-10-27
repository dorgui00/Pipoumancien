// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DB_Manager.h"

#include "F_Skeleton.h"

UDB_Manager* UDB_Manager::MyInstance;

UDB_Manager* UDB_Manager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<UDB_Manager>(); 
	}

	return MyInstance;
}

F_Skeleton* UDB_Manager::GetSkeletonByID(int ID)
{
	TArray<F_Skeleton*> Skeletons;
	DB_Music->GetAllRows("", Skeletons);
	for (const auto Skeleton : Skeletons)
	{
		if (Skeleton->ID == ID)
		{
			return Skeleton;
		};
	}
	
	return nullptr;
}
