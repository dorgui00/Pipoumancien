
#include "Tools/Slider/NoteMapping.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Modules/ModuleManager.h"
#include "Sound/SoundCue.h"

#if WITH_EDITOR

static bool StringToInput(const FString& In, ENoteInput& OutInput)
{
    if (In.Equals(TEXT("X"), ESearchCase::IgnoreCase)) { OutInput = ENoteInput::X; return true; }
    if (In.Equals(TEXT("Y"), ESearchCase::IgnoreCase)) { OutInput = ENoteInput::Y; return true; }
    if (In.Equals(TEXT("A"), ESearchCase::IgnoreCase)) { OutInput = ENoteInput::A; return true; }
    if (In.Equals(TEXT("B"), ESearchCase::IgnoreCase)) { OutInput = ENoteInput::B; return true; }
    return false;
}

void UNoteMapping::AutoBuildFromFolder()
{
    if (FolderPath.Path.IsEmpty())
    {
        UE_LOG(LogTemp, Warning, TEXT("AutoBuildFromFolder: Folder path is empty."));
        return;
    }

    FString GameRelativePath = FolderPath.Path;           // Pipoumancien/Sounds/Notes

    if (!GameRelativePath.StartsWith(TEXT("/Game")))
    {
        GameRelativePath = TEXT("/Game/") + GameRelativePath;
    }

    UE_LOG(LogTemp, Log, TEXT("AutoBuildFromFolder: Scanning path %s"), *GameRelativePath);

    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");

    TArray<FAssetData> AssetDataList;
    const bool bRecursive = true;
    AssetRegistryModule.Get().GetAssetsByPath(FName(*GameRelativePath), AssetDataList, bRecursive);

    TMap<FNoteKey, TArray<TSoftObjectPtr<USoundBase>>> TempMap;

    for (const FAssetData& AssetData : AssetDataList)
    {
        if (!AssetData.IsValid() ||
            !AssetData.GetClass()->IsChildOf(USoundBase::StaticClass()))
        {
            continue;
        }

        const FString AssetName = AssetData.AssetName.ToString();

        TArray<FString> Parts;
        AssetName.ParseIntoArray(Parts, TEXT("_"), true);

        if (Parts.Num() < 3 || !Parts[0].Equals(TEXT("Note"), ESearchCase::IgnoreCase))
        {
            continue;
        }

        const FString& PitchStr = Parts[1];
        const FString& InputStr = Parts[2];

        if (!PitchStr.IsNumeric())
        {
            continue;
        }

        int32 Pitch10Int = FCString::Atoi(*PitchStr);
        ENoteInput InputEnum;
        if (!StringToInput(InputStr, InputEnum))
        {
            continue;
        }

        FNoteKey Key;
        Key.Pitch10 = static_cast<int8>(Pitch10Int);
        Key.Input = InputEnum;

        auto& Bucket = TempMap.FindOrAdd(Key);
        Bucket.Add(TSoftObjectPtr<USoundBase>(AssetData.ToSoftObjectPath()));
    }

    NoteSets.Empty();

    for (auto& Pair : TempMap)
    {
        FNoteSoundSet Set;
        Set.Key = Pair.Key;

        Pair.Value.Sort([](const TSoftObjectPtr<USoundBase>& A,
            const TSoftObjectPtr<USoundBase>& B)
            {
                return A.ToString() < B.ToString();
            });

        if (Pair.Value.Num() > 3)
        {
            Pair.Value.SetNum(3);
        }

        Set.Variants = Pair.Value;
        NoteSets.Add(Set);
    }

    MarkPackageDirty(); //asset changed, save it pls
}


#endif

void UNoteMapping::GetNoteVariants(float Pitch, ENoteInput Input,
    TArray<USoundCue*>& OutCues) const
{
    OutCues.Reset();

    const int32 Pitch10Int = FMath::RoundToInt(Pitch * 10.f);

    FNoteKey Key;
    Key.Pitch10 = static_cast<int8>(Pitch10Int);
    Key.Input = Input;

    const FNoteSoundSet* Found = NoteSets.FindByPredicate(
        [&Key](const FNoteSoundSet& S)
        {
            return S.Key == Key;
        });

    if (!Found)
    {
        return;
    }

    for (const TSoftObjectPtr<USoundBase>& SoftSound : Found->Variants)
    {
        if (USoundBase* Base = SoftSound.LoadSynchronous())
        {
            if (USoundCue* Cue = Cast<USoundCue>(Base))
            {
                OutCues.Add(Cue);
            }
        }
    }
}


