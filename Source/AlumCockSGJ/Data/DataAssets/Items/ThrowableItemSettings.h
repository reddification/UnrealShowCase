#pragma once

#include "EquippableItemSettings.h"
#include "ThrowableItemSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UThrowableItemSettings : public UEquippableItemSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class ABaseThrowableProjectile> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EAmmunitionType AmmunitionType = EAmmunitionType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f))
	float ThrowDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* ThrowMontage = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=-90.f, ClampMin=-90.f, UIMax=90.f, ClampMax=90.f))
	float ThrowAngle = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin=10.f, ClampMin=10.f))
	float ThrowSpeed = 1500.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EThrowableSlot ThrowableSlot = EThrowableSlot::Explosive;
};