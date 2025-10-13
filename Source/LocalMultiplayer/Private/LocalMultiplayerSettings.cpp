


#include "LocalMultiplayerSettings.h"
#include "InputMappingContext.h"

// Check l'intérieur d'un mapping context donné pour trouver une key
bool FLocalMultiplayerProfileData::ContainsKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const
{
	const UInputMappingContext* MappingContext = GetIMCFromType(MappingType);
	if (MappingContext == nullptr) return false;

	for (FEnhancedActionKeyMapping KeyMapping : MappingContext->GetMappings())
	{
		if (KeyMapping.Key == Key)
		{
			return true;
		}
	}

	return false;
}

// Get un mapping context basé sur un type
UInputMappingContext* FLocalMultiplayerProfileData::GetIMCFromType(ELocalMultiplayerInputMappingType MappingType) const
{
	switch (MappingType)
	{
	case ELocalMultiplayerInputMappingType::InGame:
			return IMCInGame;
	case ELocalMultiplayerInputMappingType::Menu:
			return IMCMenu;;
		default:
			break;
	}

	return nullptr;
}

// Get le nombre de keyboard profile dans la liste des keyboard profile
int ULocalMultiplayerSettings::GetNbKeyboardProfiles() const
{
	return KeyboardProfileData.Num();
}

// Get le KeyboardProfileIndex (donc quel keyboard dans la liste le 1 ou le 2) en fonction d'une clé passé (avec un check si la clé 
// est contenu dans le mapping context du KeyprofileIndex)
int ULocalMultiplayerSettings::FindKeyboardProfileIndexFromKey(const FKey& Key, ELocalMultiplayerInputMappingType MappingType) const
{
	for (int Index = 0; Index < GetNbKeyboardProfiles(); ++Index)
	{
		if (KeyboardProfileData[Index].ContainsKey(Key, MappingType))
		{
			return Index;
		}
	}

	return -1;
}
