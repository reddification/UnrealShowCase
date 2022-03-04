#include "BaseGameplayTagActor.h"

void ABaseGameplayTagActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = GameplayTags;
}

bool ABaseGameplayTagActor::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return GameplayTags.HasTagExact(TagToCheck);
}

bool ABaseGameplayTagActor::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAllExact(TagContainer);
}

bool ABaseGameplayTagActor::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return GameplayTags.HasAnyExact(TagContainer);
}
