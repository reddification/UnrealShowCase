#pragma once

#include "WorldItemDTR.h"
#include "WeaponDTR.generated.h"

USTRUCT(BlueprintType)
struct FWeaponDTR : public FWorldItemDTR
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class AEquippableItem> EquipmentItemClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AddSupply = 20;
};