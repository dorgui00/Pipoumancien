// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DB_Manager.h"

#include "F_Skeleton.h"

ADB_Manager* ADB_Manager::MyInstance;

ADB_Manager* ADB_Manager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<ADB_Manager>(); 
	}

	return MyInstance;
}

F_Skeleton* ADB_Manager::GetSkeletonByID(int ID)
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
