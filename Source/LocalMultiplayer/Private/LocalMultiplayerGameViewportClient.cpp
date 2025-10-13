


#include "LocalMultiplayerGameViewportClient.h"
#include "LocalMultiplayerSubsystem.h"
#include "LocalMultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	Super::PostInitProperties();
	MaxSplitscreenPlayers = 8;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	Super::InputKey(EventArgs);

	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	if (Settings == nullptr) return false;

	ULocalMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	if (Subsystem == nullptr) return false;

	if (EventArgs.Key.IsGamepadKey())
	{
		int PlayerID = Subsystem->GetAssignPlayerIndexFromGamepadDeviceID(EventArgs.ControllerId);

		if (PlayerID == -1)
		{
			PlayerID = Subsystem->AssignNewPlayerToGamepadDeviceID(EventArgs.ControllerId);
			Subsystem->AssignGamepadInputMapping(PlayerID, ELocalMultiplayerInputMappingType::InGame);
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerID);
		if (PlayerController)
		{
			PlayerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));

			return true;
		}
	}
	else
	{
		int KeyboardProfileIndex = Settings->FindKeyboardProfileIndexFromKey(EventArgs.Key, ELocalMultiplayerInputMappingType::InGame);

		if (KeyboardProfileIndex != -1)
		{
			int PlayerID = Subsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);

			if (PlayerID == -1)
			{
				PlayerID = Subsystem->AssignNewPlayerToKeyboardProfile(KeyboardProfileIndex);
				Subsystem->AssignKeyboardMapping(PlayerID, KeyboardProfileIndex, ELocalMultiplayerInputMappingType::InGame);
			}

			APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerID);
			if (PlayerController)
			{
				PlayerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));

				return true;
			}
		}
	}
	
	return false;
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float Deltatime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	Super::InputAxis(InViewport, InputDevice, Key, Delta, Deltatime, NumSamples, bGamepad); 

	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	ULocalMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	int PlayerIndex = -1;

	if (bGamepad)
	{
		PlayerIndex = Subsystem->GetAssignPlayerIndexFromGamepadDeviceID(InputDevice.GetId());

		if (PlayerIndex == -1)
		{
			PlayerIndex = Subsystem->AssignNewPlayerToGamepadDeviceID(InputDevice.GetId());
			Subsystem->AssignGamepadInputMapping(PlayerIndex, ELocalMultiplayerInputMappingType::InGame);
		}

		APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
		if (PlayerController)
		{
			PlayerController->InputKey(FInputKeyParams(Key, Delta, Deltatime, NumSamples, bGamepad, InputDevice));
			return true;
		}
	}
	else
	{
		int KeyboardProfileIndex = Settings->FindKeyboardProfileIndexFromKey(Key, ELocalMultiplayerInputMappingType::InGame);
		if (KeyboardProfileIndex != -1)
		{
			PlayerIndex = Subsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);

			if (PlayerIndex == -1)
			{
				PlayerIndex = Subsystem->AssignNewPlayerToKeyboardProfile(KeyboardProfileIndex);
				Subsystem->AssignKeyboardMapping(PlayerIndex, KeyboardProfileIndex, ELocalMultiplayerInputMappingType::InGame);
			}

			APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
			if (PlayerController)
			{
				PlayerController->InputKey(FInputKeyParams(Key, Delta, Deltatime, NumSamples, bGamepad, InputDevice));
				return true;
			}
		}
	}

	return false;
}
