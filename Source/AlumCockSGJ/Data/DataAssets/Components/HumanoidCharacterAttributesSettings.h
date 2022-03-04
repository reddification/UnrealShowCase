#pragma once

#include "BaseAttributesSettings.h"
#include "HumanoidCharacterAttributesSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UHumanoidCharacterAttributesSettings : public UBaseAttributesSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.f))
	float MaxOxygen = 50.f;

	// Per second
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.f))
	float OxygenRestoreRate = 15.0f;
	
	// Per second 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.f))
	float OxygenConsumptionRate = 2.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.f))
	float DrowningHealthDamageInterval = 2.f;

	// Per DrowningHealthDamageInterval
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Oxygen", meta=(UIMin = 0.f))
	float DrowningHealthDamage = 15.f;
};