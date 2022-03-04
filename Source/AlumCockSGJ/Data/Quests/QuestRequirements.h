#pragma once
#include "QuestEnums.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "QuestRequirements.generated.h"

USTRUCT(BlueprintType)
struct FQuestRequirementQuestFilter
{
	GENERATED_BODY()
	
	// Any or each of the list
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EContainmentType FilterType;

	// In which state must quests be
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EQuestState QuestState;
	
	// List of quests DTRs
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FDataTableRowHandle> QuestsDTRs;

	// Must posses for filter to work or the opposite
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bMustPossess = true;
};

USTRUCT(BlueprintType)
struct FQuestRequirementItemFilter
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bActive = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true", RowType="WorldItemDTR"))
	TArray<FDataTableRowHandle> MustPossessItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	EContainmentType ContainmentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "bActive == true"))
	bool bMustPossess = true;
};

USTRUCT(BlueprintType)
struct FQuestRequirements
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery RequiresWorldState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery RequiresPlayerGameplayState;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestRequirementItemFilter ItemFilters;
};