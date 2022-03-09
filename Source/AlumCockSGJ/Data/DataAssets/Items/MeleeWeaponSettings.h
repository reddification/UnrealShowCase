#pragma once

#include "EquippableItemSettings.h"
#include "Data/MeleeAttackData.h"

#include "MeleeWeaponSettings.generated.h"

UCLASS()
class UMeleeWeaponSettings : public UEquippableItemSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<EMeleeAttackType, FMeleeAttackData> Attacks;
};
