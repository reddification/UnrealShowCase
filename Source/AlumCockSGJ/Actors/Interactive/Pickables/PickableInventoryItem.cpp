#include "PickableInventoryItem.h"

#include "Characters/PlayerCharacter.h"
#include "Components/Character/InventoryComponent.h"

bool APickableInventoryItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	bool bCanPickUp = Super::PickUp(PlayerCharacter, WorldItemDTR, false);
	if (!bCanPickUp)
		return false;
	
	return PlayerCharacter->GetInventoryComponent()->PickUpItem(this);
}
