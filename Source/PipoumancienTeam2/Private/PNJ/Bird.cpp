// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/Bird.h"

#include "Components/WidgetComponent.h"
#include "Data/BirdData.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Data/HUDData.h"
#include "Kismet/GameplayStatics.h"
#include "PNJ/SkeletonController.h"
#include "Settings/BirdSettings.h"
#include "Settings/SubsystemSettings.h"
#include "UI/BirdWidget.h"
#include "UI/GlobalHUDSubsystem.h"


// Sets default values
ABird::ABird()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);
	
}

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

void ABird::Init()
{
	// ---- INIT MYSELF ----
	// Not visible
	WidgetComponent->SetVisibility(false);
	WidgetComponent->InitWidget();

	// Get my widget
	BirdWidget = Cast<UBirdWidget>(WidgetComponent->GetWidget());
	if (!BirdWidget) UE_LOG(LogTemp, Error, TEXT("BirdWidget is null"));
	BirdWidget->NativeConstruct(); // to edit (but not working without)
	
	// ---- DATAS ----
	
	const USubsystemSettings* SubsystemSettings = GetDefault<USubsystemSettings>();
	UHUDData* HudData = SubsystemSettings->HUDData.LoadSynchronous();
	if (!HudData)
		UE_LOG(LogTemp, Error, TEXT("HudData is null"));

	const UBirdSettings* BirdSettings = GetDefault<UBirdSettings>();
	UBirdData* BirdData = BirdSettings->BirdData.LoadSynchronous();
	if (!BirdData)
		UE_LOG(LogTemp, Error, TEXT("BirdData is null"));
	
	SoundFromImage = {
		{HudData->NoteUp, BirdData->UpSound},
		{HudData->NoteRight, BirdData->RightSound},
		{HudData->NoteDown, BirdData->DownSound},
		{HudData->NoteLeft, BirdData->LeftSound},
	};
	
	// Utilities
	GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
	GlobalHUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();

	// ---- INIT MYSELF IN OTHERS ----
	GlobalGameSubsystem->SetBird(this);

}

void ABird::SetWidgetVisibility(bool Visibility)
{
	if (Visibility)
	{
		SetMyNotes();
		SetActorLocation(GlobalGameSubsystem->GetCurrentSkeleton()->ZoneBird->GetActorLocation());

	}
	
	// if (WidgetComponent->IsVisible() != Visibility)
	// {
		WidgetComponent->SetVisibility(Visibility);
	//}
}


// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


TObjectPtr<USoundBase> ABird::GetSoundFromImage(const UTexture2D* Texture)
{
	return SoundFromImage[Texture];
}

void ABird::PlaySound(const UTexture2D* Texture)
{
	UGameplayStatics::PlaySound2D(GetWorld(),GetSoundFromImage(Texture));
}

void ABird::SetMyNotes()
{
	// secu
	if (!GlobalGameSubsystem->GetCurrentSkeleton()) return ;
	
	F_Skeleton* Skeleton = GlobalGameSubsystem->GetCurrentSkeleton()->MySkeleton;

	// for skeleton first notes
	for (int i = 0; i<3;i++)
	{
		// Set text from input 
		UTexture2D* Text = GlobalHUDSubsystem->GetImageTextureFromNoteInput(Skeleton->Notes[i].InputAction);
		
		BirdWidget->Images[i]->SetBrushFromTexture(Text);
		BirdWidget->Images[i]->SetColorAndOpacity(FLinearColor::Green);

		
		// SOUNDS
		// TO EDIT (PLACE HOLDER)
		float Delay = 2.f*i;
		
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,	[this, Text](){PlaySound(Text);},Delay,false
		);
	}
}

void ABird::SetWidgetINVisible()
{
	WidgetComponent->SetVisibility(false);
}

