


#include "LocalMultiplayerSubsystem.h"
#include "LocalMultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystemInterface.h"

// Créer et initialise des PlayerControllers en fonction de settings clavier ou gamepad
void ULocalMultiplayerSubsystem::CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType)
{
	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	if (Settings == nullptr) return;

	for (int Index = 0; Index < Settings->GetNbKeyboardProfiles(); ++Index)
	{
		UGameplayStatics::CreatePlayer(this, Index, true);
	}

	for (int Index = 0; Index < Settings->NbMaxGamepads; ++Index)
	{
		UGameplayStatics::CreatePlayer(this, Index + Settings->GetNbKeyboardProfiles(), true);
	}
}

// Get Player Index avec une clé KeyProfileIndex
int ULocalMultiplayerSubsystem::GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex)
{
	if (PlayerIndexFromKeyboardProfileIndex.Contains(KeyboardProfileIndex))
	{
		return PlayerIndexFromKeyboardProfileIndex.FindRef(KeyboardProfileIndex);
	}

	return -1;
}

// Supposition = assigne un nouveau PlayerIndex à un KeyboardProfile
int ULocalMultiplayerSubsystem::AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex)
{
	LastAssignedPlayerIndex++;
	PlayerIndexFromKeyboardProfileIndex.Add(KeyboardProfileIndex, LastAssignedPlayerIndex);
	return LastAssignedPlayerIndex;
}

// Récupère l'IMC du KeyboardProfileIndex donné -> puis avec le PlayerIndex on récup le PlayerController pour lui assigner le mapping context
void ULocalMultiplayerSubsystem::AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	if (Settings == nullptr) return;

	// Get Player Controller and Subsystem
	APlayerController* PC = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
	if (PC == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (Subsystem == nullptr) return;

	FModifyContextOptions Options;
	Options.bForceImmediately = true;
	Subsystem->AddMappingContext(Settings->KeyboardProfileData[KeyboardProfileIndex].GetIMCFromType(MappingType), 0, Options);
}


// Get un PlayerIndex avec une clé GamepadDeviceID (GamepadProfileIndex)
int ULocalMultiplayerSubsystem::GetAssignPlayerIndexFromGamepadDeviceID(int DeviceID)
{
	if (PlayerIndexFromGamepadProfileIndex.Contains(DeviceID))
	{
		return PlayerIndexFromGamepadProfileIndex.FindRef(DeviceID);
	}

	return -1;
}

// Supposition = assigne un nouveau PlayerIndex à un GamepadDeviceID (GamepadProfile)
int ULocalMultiplayerSubsystem::AssignNewPlayerToGamepadDeviceID(int DeviceID)
{
	LastAssignedPlayerIndex++;
	PlayerIndexFromGamepadProfileIndex.Add(DeviceID, LastAssignedPlayerIndex);
	return LastAssignedPlayerIndex;
}

// Récupère l'IMC -> puis avec le PlayerIndex on récup le PlayerController pour lui assigner le mapping context
void ULocalMultiplayerSubsystem::AssignGamepadInputMapping(int PlayerIndex, ELocalMultiplayerInputMappingType MappingType) const
{
	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	if (Settings == nullptr) return;

	// Get Player Controller and Subsystem
	APlayerController* PC = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
	if (PC == nullptr) return;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (Subsystem == nullptr) return;

	FModifyContextOptions Options;
	Options.bForceImmediately = true;
	Subsystem->AddMappingContext(Settings->GamepadProfileData.GetIMCFromType(MappingType), 0, Options);
}
