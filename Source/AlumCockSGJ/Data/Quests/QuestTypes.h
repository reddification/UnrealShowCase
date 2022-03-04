#pragma once
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/EQS/Tests/EnvQueryTest_HasGameplayTags.h"
#include "Engine/DataTable.h"

#include "QuestTypes.generated.h"

USTRUCT(BlueprintType)
struct FQuestNavigationGuidanceData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery AtWorldState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery UntilWorldState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle LocationDTRH;
};

struct FQuestNavigationGuidance
{
	const AQuestLocation* QuestLocation = nullptr;
	FGameplayTagQuery UntilWorldState;
	
	void Clear()
	{
		QuestLocation = nullptr;
		UntilWorldState.Clear();
	};
};
