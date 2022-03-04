#include "PickableAmmoItem.h"
#include "Characters/PlayerCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"

bool APickableAmmoItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	bool bCanPickUp = Super::PickUp(PlayerCharacter, WorldItemDTR, false);
	if (!bCanPickUp)
		return false;
	
	bool bPickedUp = PlayerCharacter->GetEquipmentComponent()->PickUpAmmo(AmmunitionType, AddAmmoOnPickUp);
	if (bPickedUp)
	{
		PlayPickUpSound(WorldItemDTR);
		Destroy();
	}

	return bPickedUp;
}
