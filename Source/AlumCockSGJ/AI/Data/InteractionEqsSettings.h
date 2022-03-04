#pragma once
#include "GameplayTagContainer.h"
#include "NpcActivityTypes.h"

struct FInteractionEqsSettings
{
	const TArray<TSubclassOf<AActor>>* SearchedActorClasses = nullptr;
	const FGameplayTagQuery* GameplayTagQuery = nullptr;
	const TArray<FActivityIgnoreActorData>* IgnoredActors = nullptr;
};
