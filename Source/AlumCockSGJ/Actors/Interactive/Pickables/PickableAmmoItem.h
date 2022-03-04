#pragma once

#include "CoreMinimal.h"
#include "BasePickableItem.h"
#include "Data/EquipmentTypes.h"
#include "PickableAmmoItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APickableAmmoItem : public ABasePickableItem
{
	GENERATED_BODY()

protected:
	virtual bool PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int AddAmmoOnPickUp = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAmmunitionType AmmunitionType = EAmmunitionType::None;
};
