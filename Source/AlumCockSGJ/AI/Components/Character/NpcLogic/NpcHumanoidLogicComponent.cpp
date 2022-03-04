#include "NpcHumanoidLogicComponent.h"

#include "Characters/BaseCharacter.h"

void UNpcHumanoidLogicComponent::ApplyState(const UBaseNpcStateSettings* NpcState)
{
	Super::ApplyState(NpcState);
	auto HumanoidCharacterState = Cast<UNpcHumanoidStateSettings>(NpcState);
	OwnerCharacter->bSprintRequested = HumanoidCharacterState->bSprinting;
}
