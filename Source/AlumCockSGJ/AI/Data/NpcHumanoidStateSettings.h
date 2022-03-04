#pragma once
#include "DogTypes.h"

#include "NpcHumanoidStateSettings.generated.h"

UCLASS()
class UBaseNpcStateSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float MovementWalkSpeed = 300.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bStrafing = false;
};

UCLASS()
class UNpcHumanoidStateSettings : public UBaseNpcStateSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bSprinting = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bBackingOff = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFleeing = false;
};

UCLASS()
class UNpcDogStateSettings : public UBaseNpcStateSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDogAction DogAction;
};
