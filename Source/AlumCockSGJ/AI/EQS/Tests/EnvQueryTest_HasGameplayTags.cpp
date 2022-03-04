#include "EnvQueryTest_HasGameplayTags.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEnvQueryTest_HasGameplayTags::UEnvQueryTest_HasGameplayTags(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	SetWorkOnFloatValues(false);
	// ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	ValidItemType = UEnvQueryItemType_VectorBase::StaticClass();
}

bool UEnvQueryTest_HasGameplayTags::SatisfiesTest(IGameplayTagAssetInterface* ItemGameplayTagAssetInterface) const
{
	FGameplayTagContainer OwnedGameplayTags;
	ItemGameplayTagAssetInterface->GetOwnedGameplayTags(OwnedGameplayTags);

	return OwnedGameplayTags.MatchesQuery(TagQueryToMatch);
}

void UEnvQueryTest_HasGameplayTags::RunTest(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
		return;

	BoolValue.BindData(QueryOwner, QueryInstance.QueryID);
	bool bWantsValid = BoolValue.GetValue();

	// loop through all items
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		IGameplayTagAssetInterface* GameplayTagAssetInterface = Cast<IGameplayTagAssetInterface>(ItemActor);
		if (GameplayTagAssetInterface != NULL)
		{
			bool bSatisfiesTest = SatisfiesTest(GameplayTagAssetInterface);

			// bWantsValid is the basically the opposite of bInverseCondition in BTDecorator.  Possibly we should
			// rename to make these more consistent.
			It.SetScore(TestPurpose, FilterType, bSatisfiesTest, bWantsValid);
		}
		else // If no GameplayTagAssetInterface is found, this test doesn't apply at all, so just skip the item.
		{	 // Currently 
			It.ForceItemState(EEnvItemStatus::Passed);
		}
	}
}

FText UEnvQueryTest_HasGameplayTags::GetDescriptionDetails() const
{
	return FText::FromString(TagQueryToMatch.GetDescription());
}

void UEnvQueryTest_HasGameplayTags::SetTagQueryToMatch(FGameplayTagQuery& GameplayTagQuery)
{
	TagQueryToMatch = GameplayTagQuery;
}

#undef LOCTEXT_NAMESPACE