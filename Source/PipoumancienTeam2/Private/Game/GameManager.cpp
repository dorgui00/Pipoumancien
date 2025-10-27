// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/GameManager.h"

UGameManager* UGameManager::MyInstance;

UGameManager* UGameManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<UGameManager>(); 
	}

	return MyInstance;
}
