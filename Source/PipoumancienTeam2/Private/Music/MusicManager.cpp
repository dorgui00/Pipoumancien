// Fill out your copyright notice in the Description page of Project Settings.


#include "Music/MusicManager.h"

UMusicManager* UMusicManager::MyInstance;
	
UMusicManager* UMusicManager::Instance()
{
	if (!MyInstance)
	{
		MyInstance = NewObject<UMusicManager>(); 
	}

	return MyInstance;
}
