#pragma once
#include "Engine/DataTable.h"
#include "QuestRequirements.h"
#include "QuestTaskActor.h"
#include "QuestTaskTriggerCollection.generated.h"

USTRUCT(BlueprintType)
struct FReachLocation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle LocationDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle ByCharacterDTRH;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestRequirementItemFilter ItemPossessionFilter;
};

USTRUCT(BlueprintType)
struct FQuestTaskTriggerCollection
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FReachLocation LocationsReached;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestTaskKillNPCsTrigger NPCsKilled;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestTaskItemCollectionTrigger ItemsAcquired;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestTaskActor ActorsInteracted;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle ReadableRead;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestTaskNpcInteraction NpcInteracted;
};