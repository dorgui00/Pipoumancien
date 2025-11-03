#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"

#include "Data/F_Note.h"
#include "Tools/Slider/MultiHandleSlider.h"

#include "MusicDT_BPLibrary.generated.h"



UCLASS()
class PIPOUMANCIENTEAM2_API UMusicDT_BPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT")
	static bool UpsertSkeletonRow(
		UDataTable* Table,
		FName RowName,
		FName DisplayName,
		int32 ID,
		const TArray<F_Note>& Notes,
		USoundCue* SuccessMusic,
		bool bSaveAsset = true);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT")
	static bool RemoveSkeletonRow(UDataTable* Table, FName RowName, bool bSaveAsset = true);

	//retunr false if none found
	UFUNCTION(BlueprintPure, Category = "MusicDT")
	static bool GetSkeletonRow(UDataTable* Table, FName RowName, F_Skeleton& OutRow);

	//replace ONLY the notes array for existing row. creates row if doesnt exist
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT")
	static bool SetSkeletonRowNotes(UDataTable* Table, FName RowName, const TArray<F_Note>& Notes, bool bSaveAsset = true);

	//append one note to an existing rows Notes (create row if missing)
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT")
	static bool AppendNoteToRow(UDataTable* Table, FName RowName, const F_Note& Note, bool bSaveAsset = true);

	//replace ALL rows using a Name→Row map. Clears the table first.
	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT")
	static bool ReplaceAllRows(UDataTable* Table, const TMap<FName, F_Skeleton>& Rows, bool bSaveAsset = true);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "MusicDT",
		meta = (AutoCreateRefTerm = "Handles,OutNotes,ActionByKey"))
	static void BuildNotesFromHandles_Lane01Map(
		const TArray<FMultiSliderHandleSorted>& Handles,
		TArray<F_Note>& OutNotes,
		float SnapStep,
		const TMap<FName, UInputAction*>& ActionByKey);

};
