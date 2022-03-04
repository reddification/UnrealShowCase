#pragma once

#include "CoreMinimal.h"
#include "BasePickableItem.h"
#include "PickableInventoryItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API APickableInventoryItem : public ABasePickableItem
{
	GENERATED_BODY()

protected:
	virtual bool PickUp(class APlayerCharacter*, const FWorldItemDTR* WorldItemDTR, bool bForce) override;
};
