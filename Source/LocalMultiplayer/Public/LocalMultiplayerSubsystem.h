

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LocalMultiplayerSubsystem.generated.h"

enum class ELocalMultiplayerInputMappingType : uint8;

UCLASS()
class LOCALMULTIPLAYER_API ULocalMultiplayerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:	
	UFUNCTION(BlueprintCallable)
	void CreateAndInitPlayers(ELocalMultiplayerInputMappingType MappingType);
	int GetAssignedPlayerIndexFromKeyboardProfileIndex(int KeyboardProfileIndex);
	int AssignNewPlayerToKeyboardProfile(int KeyboardProfileIndex);
	void AssignKeyboardMapping(int PlayerIndex, int KeyboardProfileIndex, ELocalMultiplayerInputMappingType MappingType) const;
	int GetAssignPlayerIndexFromGamepadDeviceID(int DeviceID);
	int AssignNewPlayerToGamepadDeviceID(int DeviceID);
	void AssignGamepadInputMapping(int PlayerIndex, ELocalMultiplayerInputMappingType MappingType) const;
	
protected:
	UPROPERTY()
	uint8 LastAssignedPlayerIndex = -1;
	
	// Clé = KeyboardProfileIndex -> value = PlayerIndex
	UPROPERTY()
	TMap<int, int> PlayerIndexFromKeyboardProfileIndex;

	// Clé = GamepadProfileIndex -> value = PlayerIndex
	UPROPERTY()
	TMap<int, int> PlayerIndexFromGamepadProfileIndex;

};
