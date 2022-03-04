#include "EnvQueryGenerator_NpcInteractiveActors.h"

#include "EngineUtils.h"
#include "GameplayTagAssetInterface.h"
#include "NavigationSystem.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Data/InteractionEqsSettings.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEnvQueryGenerator_NpcInteractiveActors::UEnvQueryGenerator_NpcInteractiveActors()
{
	ItemType = UEnvQueryItemType_Actor::StaticClass();

	GenerateOnlyActorsInRadius.DefaultValue = false;
	SearchRadius.DefaultValue = 500.0f;
	SearchCenter = UEnvQueryContext_Querier::StaticClass();
}

void UEnvQueryGenerator_NpcInteractiveActors::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	UObject* QueryOwner = QueryInstance.Owner.Get();
	if (QueryOwner == nullptr)
		return;

	UWorld* World = GEngine->GetWorldFromContextObject(QueryOwner, EGetWorldErrorMode::LogAndReturnNull);
	if (World == nullptr)
		return;
	
	auto NpcOwner = Cast<APawn>(QueryInstance.Owner);
	auto ActivityProvider = Cast<INpcActivityManager>(NpcOwner->GetController());
	if (!ActivityProvider)
		return;
	
	UNpcActivityInstanceBase* Activity = ActivityProvider->GetRunningActivity();
	if (!Activity)
		return;

	auto EqsInteractionSettings = Activity->PrepareEqsInteractionSettings();
	if (!EqsInteractionSettings.SearchedActorClasses || EqsInteractionSettings.SearchedActorClasses->Num() <= 0)
		return;
	
	GenerateOnlyActorsInRadius.BindData(QueryOwner, QueryInstance.QueryID);
	bool bUseRadius = GenerateOnlyActorsInRadius.GetValue();

	TArray<AActor*> MatchingActors;
	if (bUseRadius)
	{
		TArray<FVector> ContextLocations;
		QueryInstance.PrepareContext(SearchCenter, ContextLocations);

		SearchRadius.BindData(QueryOwner, QueryInstance.QueryID);
		const float RadiusValue = SearchRadius.GetValue();
		const float RadiusSq = FMath::Square(RadiusValue);

		for (const auto& SearchedActorClass : *EqsInteractionSettings.SearchedActorClasses)
		{
			for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, SearchedActorClass); ItActor; ++ItActor)
			{
				if (!IsSuitable(*ItActor, EqsInteractionSettings))
					continue;
				
				for (int32 ContextIndex = 0; ContextIndex < ContextLocations.Num(); ++ContextIndex)
				{
					if (FVector::DistSquared(ContextLocations[ContextIndex], ItActor->GetActorLocation()) < RadiusSq)
					{
						MatchingActors.Add(*ItActor);
						break;
					}
				}
			}
		}
	}
	else
	{	// If radius is not positive, ignore Search Center and Search Radius and just return all actors of class.
		for (const auto& SearchedActorClass : *EqsInteractionSettings.SearchedActorClasses)
		{
			for (TActorIterator<AActor> ItActor = TActorIterator<AActor>(World, SearchedActorClass); ItActor; ++ItActor)
			{
				if (!IsSuitable(*ItActor, EqsInteractionSettings))
					continue;
				
				MatchingActors.Add(*ItActor);
			}
		}
	}

	QueryInstance.AddItemData<UEnvQueryItemType_Actor>(MatchingActors);
}

bool UEnvQueryGenerator_NpcInteractiveActors::IsSuitable(AActor* TestActor, const FInteractionEqsSettings& InteractionEqsSettings) const
{
	if (InteractionEqsSettings.IgnoredActors && InteractionEqsSettings.IgnoredActors->Num() > 0)
		for (const FActivityIgnoreActorData& IgnoredActor : *InteractionEqsSettings.IgnoredActors)
			if (IgnoredActor.Actor == TestActor)
				return false;

	if (!InteractionEqsSettings.GameplayTagQuery->IsEmpty())
	{
		IGameplayTagAssetInterface* GameplayTagActor = Cast<IGameplayTagAssetInterface>(TestActor);
		if (TestActor)
		{
			FGameplayTagContainer ActorTags;
			GameplayTagActor->GetOwnedGameplayTags(ActorTags);
			if (!ActorTags.MatchesQuery(*InteractionEqsSettings.GameplayTagQuery))
				return false;
		}
	}
	
	return true;
}

FText UEnvQueryGenerator_NpcInteractiveActors::GetDescriptionTitle() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("DescriptionTitle"), Super::GetDescriptionTitle());

	if (!GenerateOnlyActorsInRadius.IsDynamic() && !GenerateOnlyActorsInRadius.GetValue())
	{
		return FText::Format(LOCTEXT("DescriptionGenerateActors", "{DescriptionTitle}: generate set of actors of {ActorsClass}"), Args);
	}

	Args.Add(TEXT("DescribeContext"), UEnvQueryTypes::DescribeContext(SearchCenter));
	return FText::Format(LOCTEXT("DescriptionGenerateActorsAroundContext", "{DescriptionTitle}: generate set of actors of {ActorsClass} around {DescribeContext}"), Args);
};

FText UEnvQueryGenerator_NpcInteractiveActors::GetDescriptionDetails() const
{
	FFormatNamedArguments Args;
	Args.Add(TEXT("Radius"), FText::FromString(SearchRadius.ToString()));

	FText Desc = FText::Format(LOCTEXT("ActorsOfClassDescription", "radius: {Radius}"), Args);

	return Desc;
}

#undef LOCTEXT_NAMESPACE
