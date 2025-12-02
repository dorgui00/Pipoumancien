
#include "Tools/MusicDT_BPLibrary.h"

#include "Engine/DataTable.h"
#include "Data/F_Note.h"
#include "Data/F_Skeleton.h"
#include "Sound/SoundCue.h"
#include "InputAction.h"
#include "Sound/SoundBase.h"

#if WITH_EDITOR
#include "ScopedTransaction.h"
#include "EditorAssetLibrary.h"
#endif


static void SaveIf(UObject* Asset, bool bSave)
{
#if WITH_EDITOR
	if (bSave && Asset)
	{
		UEditorAssetLibrary::SaveLoadedAsset(Asset);
	}
#endif
}

bool UMusicDT_BPLibrary::UpsertSkeletonRow(UDataTable* Table, FName RowName, FName DisplayName, int32 ID, const TArray<F_Note>& Notes, USoundCue* SuccessMusic, bool bSaveAsset)
{
	if (!Table) return false;

#if WITH_EDITOR
	const FScopedTransaction Tx(NSLOCTEXT("MusicDT", "UpsertSkeletonRow", "Upsert Skeleton Row"));
	Table->Modify();
#endif

	F_Skeleton Row;
	Row.Name = DisplayName;
	Row.ID = ID;
	Row.Notes = Notes;
	Row.SuccessMusic = SuccessMusic;

	//overwrite if row alr exists
	Table->AddRow(RowName, Row);

	SaveIf(Table, bSaveAsset);
	return true;
}

bool UMusicDT_BPLibrary::RemoveSkeletonRow(UDataTable* Table, FName RowName, bool bSaveAsset)
{
	if (!Table) return false;

#if WITH_EDITOR
	const FScopedTransaction Tx(NSLOCTEXT("MusicDT", "RemoveSkeletonRow", "Remove Skeleton Row"));
	Table->Modify();
#endif

	const bool bExisted = (Table->FindRow<F_Skeleton>(RowName, TEXT("RemoveSkeletonRow")) != nullptr);

	Table->RemoveRow(RowName);

	if (bExisted)
	{
		SaveIf(Table, bSaveAsset);
	}
	return bExisted;
}


bool UMusicDT_BPLibrary::GetSkeletonRow(UDataTable* Table, FName RowName, F_Skeleton& OutRow)
{
	if (!Table) return false;

	if (const F_Skeleton* Found = Table->FindRow<F_Skeleton>(RowName, TEXT("GetSkeletonRow")))
	{
		OutRow = *Found;
		return true;
	}
	return false;
}

bool UMusicDT_BPLibrary::SetSkeletonRowNotes(UDataTable* Table, FName RowName, const TArray<F_Note>& Notes, bool bSaveAsset)
{
	if (!Table) return false;

#if WITH_EDITOR
	const FScopedTransaction Tx(NSLOCTEXT("MusicDT", "SetSkeletonRowNotes", "Set Skeleton Row Notes"));
	Table->Modify();
#endif

	F_Skeleton Row;
	if (const F_Skeleton* Found = Table->FindRow<F_Skeleton>(RowName, TEXT("SetSkeletonRowNotes")))
	{
		Row = *Found;
	}
	Row.Notes = Notes;

	Table->AddRow(RowName, Row);
	SaveIf(Table, bSaveAsset);
	return true;
}

bool UMusicDT_BPLibrary::AppendNoteToRow(UDataTable* Table, FName RowName, const F_Note& Note, bool bSaveAsset)
{
	if (!Table) return false;

#if WITH_EDITOR
	const FScopedTransaction Tx(NSLOCTEXT("MusicDT", "AppendNoteToRow", "Append Note To Skeleton Row"));
	Table->Modify();
#endif

	F_Skeleton Row;
	if (const F_Skeleton* Found = Table->FindRow<F_Skeleton>(RowName, TEXT("AppendNoteToRow")))
	{
		Row = *Found;
	}
	Row.Notes.Add(Note);

	Table->AddRow(RowName, Row);
	SaveIf(Table, bSaveAsset);
	return true;
}

bool UMusicDT_BPLibrary::ReplaceAllRows(UDataTable* Table, const TMap<FName, F_Skeleton>& Rows, bool bSaveAsset)
{
	if (!Table) return false;

#if WITH_EDITOR
	const FScopedTransaction Tx(NSLOCTEXT("MusicDT", "ReplaceAllRows", "Replace All Skeleton Rows"));
	Table->Modify();
#endif

	Table->EmptyTable();

	for (const TPair<FName, F_Skeleton>& Pair : Rows)
	{
		Table->AddRow(Pair.Key, Pair.Value);
	}

	SaveIf(Table, bSaveAsset);
	return true;
}

static float SnapToStep(float V, float Step)
{
	return (Step > 0.f)
		? FMath::Clamp(FMath::RoundToFloat(V / Step) * Step, 0.f, 1.f)
		: V;
}

static float LaneToValue(int32 Lane)
{
	switch (Lane)
	{
	case 0: return  1.0f;
	case 1: return  0.5f;
	case 2: return  0.0f;
	case 3: return -0.5f;
	case 4: return -1.0f;
	default: return 0.0f;
	}
}

void UMusicDT_BPLibrary::BuildNotesFromHandles_Lane01Map(
	const TArray<FMultiSliderHandleSorted>& Handles,
	TArray<F_Note>& OutNotes,
	float SnapStep,
	const TMap<FName, UInputAction*>& ActionByKey)
{
	OutNotes.Reset();
	OutNotes.Reserve(Handles.Num());

	for (const FMultiSliderHandleSorted& H : Handles)
	{
		F_Note Note;

		Note.Frequency = SnapToStep(H.Distance, SnapStep);
		Note.Pitch = LaneToValue(H.Lane);

		if (UInputAction* const* FoundIA = ActionByKey.Find(H.InputKey))
		{
			Note.InputAction = *FoundIA;
		}
		else
		{
			Note.InputAction = nullptr;
		}

		Note.Sound = nullptr;

		OutNotes.Add(Note);
	}
}

bool UMusicDT_BPLibrary::UpdateSkeletonRowNotes(UDataTable* Table, FName RowName, const TArray<USoundCue*>& NewCues, bool bSaveAsset)
{
	if (!Table)
	{
		return false;
	}

	F_Skeleton* RowPtr = Table->FindRow<F_Skeleton>(RowName, TEXT("UpdateSkeletonRowNotes"));
	if (!RowPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSkeletonRowNotes: Row %s not found"), *RowName.ToString());
		return false;
	}

	F_Skeleton& Row = *RowPtr;

	if (Row.Notes.Num() != NewCues.Num())
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSkeletonRowNotes: Size mismatch)"));
	}

	const int32 Count = FMath::Min(Row.Notes.Num(), NewCues.Num());

	for (int32 i = 0; i < Count; ++i)
	{
		if (NewCues[i])
		{
			Row.Notes[i].Sound = NewCues[i];
		}
	}

#if WITH_EDITOR

	Table->Modify();

#endif

	Table->AddRow(RowName, Row); // re-add

	SaveIf(Table, bSaveAsset);

	UE_LOG(LogTemp, Log, TEXT("UpdateSkeletonRowNotes: Updated %d notes in row %s"), Count, *RowName.ToString());
	return true;
}



