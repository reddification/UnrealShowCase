#pragma once
#include "QuestDialogue.h"
#include "QuestEnums.h"
#include "QuestTaskReward.h"
#include "Engine/DataTable.h"
#include "QuestTaskTriggerCollection.h"
#include "QuestAction.h"
#include "QuestTaskDTR.generated.h"

USTRUCT(BlueprintType)
struct FQuestTaskDTR : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bActive = true;

	// implicit quest tasks don't show up in journal
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bImplicit = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	EQuestState QuestState = EQuestState::InProgress;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	FQuestTaskReward QuestEventReward;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	FQuestTaskTriggerCollection TaskCompletedTriggers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	bool bExecuteActionsWhenCovered = false;
	
	// Actions to do after this task is completed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	FQuestActions TaskOccuredActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	FQuestDialogue Dialogue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="WorldState", meta = (EditCondition = "bActive == true"))
	FGameplayTagQuery CoveredByWorldState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	FQuestRequirements QuestTaskRequirements;
};
