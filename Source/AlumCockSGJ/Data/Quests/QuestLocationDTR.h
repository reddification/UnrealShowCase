#pragma once
#include "Engine/DataTable.h"
#include "QuestLocationDTR.generated.h"

USTRUCT(BlueprintType)
struct FQuestLocationDTR : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bQuestLocation = false;
};