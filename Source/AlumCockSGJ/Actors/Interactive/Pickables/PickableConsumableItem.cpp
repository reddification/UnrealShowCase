#include "PickableConsumableItem.h"
#include "Characters/PlayerCharacter.h"
#include "Components/Character/SoakingComponent.h"

bool APickableConsumableItem::PickUp(APlayerCharacter* PlayerCharacter, const FWorldItemDTR* WorldItemDTR, bool bForce)
{
	Super::PickUp(PlayerCharacter, WorldItemDTR, false);
	auto AttributesComponent = PlayerCharacter->GetBaseCharacterAttributesComponent();
	auto SoakingComponent = PlayerCharacter->GetSoakingComponent();
	bool bAppliedHealth = AttributesComponent->TryAddHealth(HealthGain);
	bool bAppliedSoakness = SoakingComponent->TryApplySoakness(SoaknessGain);
	if (bAppliedHealth || bAppliedSoakness)
	{
		PlayPickUpSound(WorldItemDTR);
		Destroy();
	}

	return bAppliedHealth || bAppliedSoakness;
}
