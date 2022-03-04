#pragma once
#include "WorldItemDTR.h"
#include "Engine/DataTable.h"

#include "ReadableDTR.generated.h"

USTRUCT()
struct FReadableDTR : public FWorldItemDTR
{
	GENERATED_BODY()

	FReadableDTR() { ItemType = EItemType::Readable; }

	// Max suggested length 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine = true))
	TArray<FText> Pages;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDataTableRowHandle> BeginQuests;
};