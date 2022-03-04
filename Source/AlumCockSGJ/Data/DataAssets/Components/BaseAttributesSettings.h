#pragma once

#include "BaseAttributesSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseAttributesSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Health", meta=(UIMin = 0.f))
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bReceiveTeamDamage = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bReceivesDamage = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxStamina = 100.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaRestoreVelocity = 20.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SprintStaminaConsumptionRate = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float JumpStaminaConsumption = 10.f;
};