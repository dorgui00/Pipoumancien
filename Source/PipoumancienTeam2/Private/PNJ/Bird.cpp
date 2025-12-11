// Fill out your copyright notice in the Description page of Project Settings.


#include "PNJ/Bird.h"

#include "Components/WidgetComponent.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "PNJ/SkeletonController.h"
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

// Called when the game starts or when spawned
void ABird::BeginPlay()
{
	Super::BeginPlay();
	
	Init();
}

// Called every frame
void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	
	// Utilities
	GlobalGameSubsystem = GetGameInstance()->GetSubsystem<UGlobalGameSubsystem>();
	GlobalHUDSubsystem = GetGameInstance()->GetSubsystem<UGlobalHUDSubsystem>();

	// ---- INIT MYSELF IN OTHERS ----
	GlobalGameSubsystem->SetBird(this);
}

void ABird::SetMyNotes() const
{
	// secu
	if (!GlobalGameSubsystem->GetCurrentSkeleton()) return ;
	
	F_Skeleton* Skeleton = GlobalGameSubsystem->GetCurrentSkeleton()->MySkeleton;

	// for skeleton first notes
	for (int i = 0; i<3;i++)
	{
		// Set text from input 
		UTexture2D* Text = GlobalHUDSubsystem->GetImageBirdTextureFromNoteInput(Skeleton->Notes[i].InputAction);
		
		BirdWidget->Images[i]->SetBrushFromTexture(Text);
		BirdWidget->Images[i]->SetColorAndOpacity(FLinearColor::Green);
	}
}

void ABird::SetWidgetINVisible()
{
	WidgetComponent->SetVisibility(false);
}

