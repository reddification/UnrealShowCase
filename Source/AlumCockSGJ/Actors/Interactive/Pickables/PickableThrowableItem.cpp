#include "PickableThrowableItem.h"

#include "Characters/PlayerCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"

bool APickableThrowableItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	return false;
	// Super::PickUp(PlayerCharacter, WorldItemDTR, false);
	// order matters for throwables
	// bool bPickedUp = PlayerCharacter->GetEquipmentComponent()->PickUpThrowable(ThrowableItemClass, AddSupply);
	// if (bPickedUp)
	// {
	// 	PlayPickUpSound(WorldItemDTR);
	// 	Destroy();
	// }

	// return bPickedUp;
}
