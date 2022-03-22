#include "WorldStateSubsystem.h"

void UWorldStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// TODO load world state
}

void UWorldStateSubsystem::ChangeWorldState(const FGameplayTagContainer& TagsContainer, bool bAdd)
{
	if (bAdd)
		WorldState.AppendTags(TagsContainer);
	else
		WorldState.RemoveTags(TagsContainer);
	
	if (WorldStateChangedEvent.IsBound())
		WorldStateChangedEvent.Broadcast(WorldState);
}

bool UWorldStateSubsystem::IsWorldStateMatches(const FGameplayTagQuery& WorldStateTagQuery) const
{
	return WorldStateTagQuery.IsEmpty() || WorldStateTagQuery.Matches(WorldState);
}

void UWorldStateSubsystem::Load()
{
	WorldState.Reset();
}
