
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
#if WITH_EDITOR
    NoteSets.Reset();

    FString Folder = FolderPath.Path;
    if (Folder.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("UNoteMapping::AutoBuildFromFolder - FolderPath is empty"));
        return;
    }

    if (!Folder.StartsWith(TEXT("/Game")))
    {
        Folder = TEXT("/Game/") + Folder;
    }

    FARFilter Filter;
    Filter.bRecursivePaths = true;
    Filter.PackagePaths.Add(*Folder);
    Filter.ClassPaths.Add(USoundCue::StaticClass()->GetClassPathName());

    TArray<FAssetData> Assets;
    FAssetRegistryModule& AssetRegistryModule =
        FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    AssetRegistryModule.Get().GetAssets(Filter, Assets);

    for (const FAssetData& AssetData : Assets)
    {
        const FString Name = AssetData.AssetName.ToString();

        UE_LOG(LogTemp, Log, TEXT("Found asset: %s"), *Name);

        TArray<FString> Parts;
        Name.ParseIntoArray(Parts, TEXT("_"));

        if (Parts.Num() < 3)
        {
            continue;
        }

        const FString& Prefix = Parts[0];
        const FString& PitchStr = Parts[1];
        const FString& InputStr = Parts[2];

        if (!Prefix.Equals(TEXT("Note"), ESearchCase::IgnoreCase) &&
            !Prefix.Equals(TEXT("Notes"), ESearchCase::IgnoreCase))
        {
            continue;
        }

        if (!PitchStr.IsNumeric())
        {
            continue;
        }

        const float ParsedPitch = (FCString::Atof(*PitchStr) / 10.0f) *10.f;

        ENoteInput InputEnum;
        if (!StringToInput(InputStr, InputEnum))
        {
            continue;
        }

        FNoteKey Key;
        Key.Pitch10 = static_cast<int8>(FMath::RoundToInt(ParsedPitch * 10.0f));
        Key.Input = InputEnum;

        FSoftObjectPath SoftPath = AssetData.ToSoftObjectPath();
        TSoftObjectPtr<USoundBase> SoundRef(SoftPath);

        FNoteSoundSet* ExistingSet = NoteSets.FindByPredicate(
            [&Key](const FNoteSoundSet& S)
            {
                return S.Key == Key;
            });

        if (ExistingSet)
        {
            ExistingSet->Variants.AddUnique(SoundRef);
        }
        else
        {
            FNoteSoundSet NewSet;
            NewSet.Key = Key;
            NewSet.Pitch = ParsedPitch;
            NewSet.Variants.Add(SoundRef);
            NoteSets.Add(MoveTemp(NewSet));
        }
    }

    NoteSets.Sort(
        [](const FNoteSoundSet& A, const FNoteSoundSet& B)
        {
            if (A.Key.Pitch10 == B.Key.Pitch10)
            {
                return (int32)A.Key.Input < (int32)B.Key.Input;
            }
            return A.Key.Pitch10 < B.Key.Pitch10;
        });

    UE_LOG(LogTemp, Log,
        TEXT("UNoteMapping::AutoBuildFromFolder - built %d sets from '%s'"),
        NoteSets.Num(), *Folder);
#endif
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


