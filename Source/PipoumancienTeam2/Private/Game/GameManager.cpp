// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

AGameManager* AGameManager::MyInstance;

AGameManager* AGameManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<AGameManager>(); 
	}

	return MyInstance;
}
