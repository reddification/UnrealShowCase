#include "NpcActivityIndefinitelyInteractWithActorInstance.h"

FInteractionEqsSettings UNpcActivityIndefinitelyInteractWithActorInstance::PrepareEqsInteractionSettings()
{
	FInteractionEqsSettings Result;
	auto Settings = GetSettings();
	Result.GameplayTagQuery = &Settings->WithTags;
	Result.SearchedActorClasses = &Settings->ActorClassToInteractWith;
	return Result;
}

FGameplayTag UNpcActivityIndefinitelyInteractWithActorInstance::GetInteractionTag()
{
	auto Settings = GetSettings();
	return Settings->InteractionTag;
}

const UNpcActivityIndefinitelyInteractWithActorSettings* UNpcActivityIndefinitelyInteractWithActorInstance::GetSettings()
{
	if (!IndefiniteInteractionSettings)
		IndefiniteInteractionSettings = Cast<UNpcActivityIndefinitelyInteractWithActorSettings>(ActivitySettings);

	return IndefiniteInteractionSettings;
}
