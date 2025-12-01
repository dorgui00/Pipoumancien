#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundBase.h"
#include "NoteMapping.generated.h"

UENUM(BlueprintType)
enum class ENoteInput : uint8
{
    X UMETA(DisplayName = "X"),
    Y UMETA(DisplayName = "Y"),
    A UMETA(DisplayName = "A"),
    B UMETA(DisplayName = "B"),
};

USTRUCT(BlueprintType)
struct FNoteKey
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Note")
    int8 Pitch10 = 0;

    UPROPERTY(EditAnywhere, Category = "Note")
    ENoteInput Input = ENoteInput::X;

    bool operator==(const FNoteKey& Other) const
    {
        return Pitch10 == Other.Pitch10 && Input == Other.Input;
    }
};

FORCEINLINE uint32 GetTypeHash(const FNoteKey& Key)
{
    return HashCombine(::GetTypeHash(Key.Pitch10),
        ::GetTypeHash(static_cast<uint8>(Key.Input)));
}

USTRUCT(BlueprintType)
struct FNoteSoundSet
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category = "Note")
    FNoteKey Key;

    UPROPERTY(EditAnywhere, Category = "Note")
    TArray<TSoftObjectPtr<USoundBase>> Variants;
};

UCLASS(BlueprintType)
class PIPOUMANCIENTEAM2_API UNoteMapping : public UDataAsset
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, Category = "Note", meta = (RelativeToGameContentDir))
    FDirectoryPath FolderPath;

#if WITH_EDITOR  //button

    UFUNCTION(CallInEditor, Category = "Note")
    void AutoBuildFromFolder();

#endif

    UPROPERTY(EditAnywhere, Category = "Note")
    TArray<FNoteSoundSet> NoteSets;

    UFUNCTION(BlueprintPure, Category = "Note")
    void GetNoteVariants(float Pitch, ENoteInput Input,
        TArray<USoundCue*>& OutCues) const;
};
