#include "BaseAnimNotify.h"

#include "Characters/BaseHumanoidCharacter.h"

ABaseHumanoidCharacter* UBaseAnimNotify::GetCharacter(AActor* Actor) const
{
	ABaseHumanoidCharacter* Character = Cast<ABaseHumanoidCharacter>(Actor);
	if (!IsValid(Character))
	{
		return nullptr;
	}

	return Character;
}
