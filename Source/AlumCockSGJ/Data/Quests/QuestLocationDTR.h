#pragma once
#include "Engine/DataTable.h"
#include "QuestLocationDTR.generated.h"

USTRUCT(BlueprintType)
struct FQuestLocationDTR : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Description;
};