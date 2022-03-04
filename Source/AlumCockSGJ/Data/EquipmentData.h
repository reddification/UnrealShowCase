#pragma once

#include "EquipmentTypes.h"
#include "EquipmentData.generated.h"

class AEquippableItem;

USTRUCT()
struct FEquipmentData
{
   GENERATED_BODY()
public:
	AEquippableItem* OldItem;
	AEquippableItem* NewItem;
	EEquipmentSlot EquipmentSlot;
	class UAnimMontage* Montage;
	bool bNotified;
};
