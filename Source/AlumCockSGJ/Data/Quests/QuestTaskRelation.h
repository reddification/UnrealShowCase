#pragma once

#include "QuestEnums.h"
#include "Engine/DataTable.h"
#include "QuestTaskRelation.generated.h"

USTRUCT(BlueprintType)
struct FQuestTaskPointer
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle QuestDTRH;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDataTableRowHandle QuestTaskDTRH;
};

USTRUCT(BlueprintType)
struct FQuestTaskRelation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EContainmentType ContainmentType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bContains;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestTaskPointer> QuestTaskPointers;
};