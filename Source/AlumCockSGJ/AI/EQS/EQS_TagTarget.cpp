#include "EQS_TagTarget.h"

void AEQS_TagTarget::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTagContainer;
}

bool AEQS_TagTarget::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayTagContainer.HasTag(TagToCheck);
}

bool AEQS_TagTarget::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTagContainer.HasAllExact(TagContainer);
}

bool AEQS_TagTarget::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTagContainer.HasAnyExact(TagContainer);
}
