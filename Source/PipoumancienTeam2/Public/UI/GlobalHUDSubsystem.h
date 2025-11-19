// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UMusicNote.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Components/TextBlock.h"
#include "GlobalHUDSubsystem.generated.h"

struct F_Note;
class USlider;
class UMusicWorldSubsystem;
class ASkeletonController;
struct F_Skeleton;
class UInputAction;
class UResurrectionWidget;
class UPipouCharacterInputData;
class UGlobalGameSubsystem;

UCLASS()
class PIPOUMANCIENTEAM2_API UGlobalHUDSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	#pragma region Music
	// Resurrection Widget Class
	UPROPERTY()
	TSubclassOf<UResurrectionWidget> WBPResurrectionClass;

	// Resurrection Widget Object (Use to store the creation of the widget with the Resurrection Widget Class)
	UPROPERTY()
	UResurrectionWidget* WBPResurrectionInstance;

	// Display the resurrection widget to the screen.
	void DisplayResurrectionWidget();

	// Remove the resurrection widget from screen.
	void RemoveResurrectionWidget();

	// Note Widget Class.
	UPROPERTY()
	TSubclassOf<UMusicNote> WBPNoteClass;

	// Note Widget Object (Use to store the creation of the widget with the Note Widget Class)
	UPROPERTY()
	UMusicNote* WBPNoteInstance;

	// Spawn the note in the NoteBox of the partition from the Skeleton Current Notes.
	void SpawnNotesPartition(const ASkeletonController* CurrentSkeleton);

	// Move the partition at the same time of the main music mechanic in MusicWorldSubsystem.
	void MovePartition(float DeltaTime);

	// Move UI backwards depending on the position of the note the player has to play (Only if he lost the qte) 
	void RewindPartition(int CurrentNoteIndex, const F_Note& NewNote);

	// Initialize by the Subsystem Settings in the Init function.
	float RatioDistance = 0;

	// UI Speed calculate each time UI moves during music. 
	float UISpeed = 0;
	
	// Size of the partition UI.
	float UiOffset;

	// Moving Start and End Point of the NotesBox.
	float MovementStartPoint = 0.f;
	float MovementEndPoint = 0.f;

	// Store the distance of all the frequencies when spawning notes.
	float DistancePreviousFrequencies;

	// The time we let the player have before having to replay
	float PreviewTime = 1.f;

	// Store the note widget spawned.
	UPROPERTY()
	TArray<UMusicNote*> NotesInstanciated;

	// Canvas Panel Slot of the Note Instance. 
	UPROPERTY()
	UCanvasPanelSlot* NotesBoxSlot;

	// Set MusicWorldSubsystem from MusicWorldSubsystem. 
	UPROPERTY()
	UMusicWorldSubsystem* MusicWorldSubsystem;

	#pragma endregion
	
	#pragma region Utilities
	// Use to store the Input Data
	UPROPERTY()
	TObjectPtr<UPipouCharacterInputData> InputData;
	
	// Set Color of an object to snap and reset the color.
	float TimerForResetingColor = 0;

	// Timer event to reset the colors after a number of seconds.
	FTimerHandle ResetColorTimerHandle;

	// Store the color link to an object.
	UPROPERTY()
	TMap<UObject*, FTimerHandle> ColorResetTimers;

	// My template function to be called for either changing color of a slider handle or one of my note 
	template<class T>
	void SetObjectColor(T* CurrentObject, FLinearColor NewColor, bool HasAchievedInput)
	{
		if (!CurrentObject) return;

		// Depending on the object I change the color differently. 
		if constexpr (std::is_same_v<T, UMusicNote>)
		{
			CurrentObject->NoteImage->SetColorAndOpacity(NewColor);
			CurrentObject->LetterText->SetColorAndOpacity(FLinearColor::Black);
		}

		if constexpr (std::is_same_v<T, USlider>)
		{
			FSliderStyle Style = CurrentObject->GetWidgetStyle();
			FSlateBrush ThumbBrush = Style.NormalThumbImage;

			ThumbBrush.OutlineSettings.Color = NewColor; 
			Style.SetNormalThumbImage(ThumbBrush);

			CurrentObject->SetWidgetStyle(Style);
		}

		// Add to my map the current object and associate it with handle or find it if there is one.
		FTimerHandle& Handle = ColorResetTimers.FindOrAdd(CurrentObject);

		// Clear all the timer in case one is started.
		GetWorld()->GetTimerManager().ClearTimer(Handle);

		// Remove the pointer to get only the object and store it in case it is destroyed.
		using TObjectClass = std::remove_pointer_t<T>;
		TWeakObjectPtr<TObjectClass> WeakObj = CurrentObject;

		bool AchievedInput = HasAchievedInput;
		
		// Create a timer with our timer event on our WeakObj.
		GetWorld()->GetTimerManager().SetTimer(
			Handle,
			[WeakObj, AchievedInput]()
			{
				if (!WeakObj.IsValid()) return;

				// Get back the object in pointer.
				T* Obj = WeakObj.Get();

				// Set a default color: blue.
				FLinearColor DefaultColor = AchievedInput ? FLinearColor::Gray : FLinearColor::Blue;

				// Check if it's a note or a slider to change the color or the slider handle color depending on the object.
				if constexpr (std::is_same_v<T, UMusicNote>)
				{
					Obj->NoteImage->SetColorAndOpacity(DefaultColor);
					Obj->LetterText->SetColorAndOpacity(FLinearColor::White);
				}

				if constexpr (std::is_same_v<T, USlider>)
				{
					FSliderStyle Style = WeakObj->GetWidgetStyle();
					FSlateBrush ThumbBrush = Style.NormalThumbImage;

					ThumbBrush.OutlineSettings.Color = FLinearColor::Blue; 
					Style.SetNormalThumbImage(ThumbBrush);

					WeakObj->SetWidgetStyle(Style);
				}
			},
			// It will start after 0.2f seconds of wait. 
			0.2f,
			false
		);
	}

	#pragma endregion 
	
protected:
	#pragma region GameInstanceSubsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	#pragma endregion

	#pragma region FTickableGameObject
	virtual UWorld* GetTickableGameObjectWorld() const override { return GetWorld(); }
	virtual ETickableTickType GetTickableTickType() const override { return ETickableTickType::Always; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { return TStatId(); };

	#pragma endregion
	
private:
	#pragma region Utilities
	// Map to associate an InputAction (Key of Controller) to a MusicNoteType (The notes in ENUM).
	UPROPERTY()
	TMap<UInputAction*, EMusicNoteType> MusicNoteFromInputAction;

	// Get the MusicNoteType with a key input action based on the map.
	EMusicNoteType GetMusicNoteTypeFromInputAction(const UInputAction* InputAction) const;

	// Game Subsystem
	UPROPERTY()
	UGlobalGameSubsystem* GlobalGameSubsystem;

	// Initialize data for GlobalHUDSubsystem.
	void Init();

	#pragma endregion

};
