// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/DB_Manager.h"
#include "EngineUtils.h"
#include "F_Skeleton.h"
#include "Kismet/GameplayStatics.h"

ADB_Manager* ADB_Manager::MyInstance;

ADB_Manager* ADB_Manager::Instance(UWorld* World)
{
	if (IsValid(MyInstance))
		return MyInstance;

	for (TActorIterator<ADB_Manager> It(World); It; ++It)
	{
		MyInstance = *It;
		break;
	}

	if (!MyInstance)
	{
		MyInstance = World->SpawnActor<ADB_Manager>(ADB_Manager::StaticClass());
	}

	return MyInstance;
}

void ADB_Manager::BeginPlay()
{
	Super::BeginPlay();
	
	if (!MyInstance)
	{
		MyInstance = Cast<ADB_Manager>(UGameplayStatics::GetActorOfClass(GetWorld(), ADB_Manager::StaticClass()));
	}

}

F_Skeleton* ADB_Manager::GetSkeletonByID(int ID)
{
	TArray<F_Skeleton*> Skeletons;
	
	if (DB_Music)
	{
		DB_Music->GetAllRows("", Skeletons);
	
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
