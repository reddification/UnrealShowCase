#pragma once
#include "Data/EquipmentTypes.h"
#include "Data/UserInterfaceTypes.h"

#include "EquippableItemSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UEquippableItemSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquippableItemType EquippableItemType = EEquippableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CharacterEquippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName CharacterUnequippedSocketName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterEquipMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimMontage* CharacterUnequipMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float EquipmentDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float UnequipDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EReticleType ReticleType = EReticleType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EEquipmentSlot DesignatedSlot = EEquipmentSlot::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bPreferStrafing = true;
};
