#pragma once
#include "Data/Entities/WorldItemDTR.h"
#include "QuestTaskReward.generated.h"

USTRUCT(BlueprintType)
struct FQuestTaskReward
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FWorldItemDTR> Rewards;
};