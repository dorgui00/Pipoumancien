#include "LocalMultiplayerGameViewportClient.h"

//#include "InterchangeResult.h"
#include "LocalMultiplayerSubsystem.h"
#include "LocalMultiplayerSettings.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerInput.h"
#include "Logging/StructuredLog.h"

void ULocalMultiplayerGameViewportClient::PostInitProperties()
{
	Super::PostInitProperties();
	MaxSplitscreenPlayers = 4;
}

bool ULocalMultiplayerGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
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
			return PlayerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));
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
				return PlayerController->InputKey(FInputKeyParams(EventArgs.Key, EventArgs.Event, static_cast<double>(EventArgs.AmountDepressed), EventArgs.IsGamepad(), EventArgs.InputDevice));
			}
		}
	}
	
	return Super::InputKey(EventArgs);
}

bool ULocalMultiplayerGameViewportClient::InputAxis(FViewport* InViewport, FInputDeviceId InputDevice, FKey Key, float Delta, float Deltatime, int32 NumSamples /*= 1*/, bool bGamepad /*= false*/)
{
	const ULocalMultiplayerSettings* Settings = GetDefault<ULocalMultiplayerSettings>();
	ULocalMultiplayerSubsystem* Subsystem = GetGameInstance()->GetSubsystem<ULocalMultiplayerSubsystem>();
	
	if (bGamepad)
	{
		int PlayerIndex = Subsystem->GetAssignPlayerIndexFromGamepadDeviceID(InputDevice.GetId());

		if (PlayerIndex == -1)
		{
			PlayerIndex = Subsystem->AssignNewPlayerToGamepadDeviceID(InputDevice.GetId());
			Subsystem->AssignGamepadInputMapping(PlayerIndex, ELocalMultiplayerInputMappingType::InGame);
		}
    
		APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
		if (PlayerController)
		{
			return PlayerController->InputKey(FInputKeyParams(Key, Delta, Deltatime, NumSamples, bGamepad, InputDevice));
		}
	}
	else
	{
		int KeyboardProfileIndex = Settings->FindKeyboardProfileIndexFromKey(Key, ELocalMultiplayerInputMappingType::InGame);
		
		if (KeyboardProfileIndex != -1)
		{
			int PlayerIndex = Subsystem->GetAssignedPlayerIndexFromKeyboardProfileIndex(KeyboardProfileIndex);

			if (PlayerIndex == -1)
			{
				PlayerIndex = Subsystem->AssignNewPlayerToKeyboardProfile(KeyboardProfileIndex);
				Subsystem->AssignKeyboardMapping(PlayerIndex, KeyboardProfileIndex, ELocalMultiplayerInputMappingType::InGame);
			}

			APlayerController* PlayerController = UGameplayStatics::GetPlayerControllerFromID(this, PlayerIndex);
			if (PlayerController)
			{
				return PlayerController->InputKey(FInputKeyParams(Key, Delta, Deltatime, NumSamples, bGamepad, InputDevice));
			}
		}
	}

	return Super::InputAxis(InViewport, InputDevice, Key, Delta, Deltatime, NumSamples, bGamepad); 
}
