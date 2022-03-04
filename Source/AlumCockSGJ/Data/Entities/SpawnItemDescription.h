#pragma once

#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "SpawnItemDescription.generated.h"

USTRUCT(BlueprintType)
struct FSpawnItemDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer WithTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle WorldItemDTRH;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin=0.f, UIMin=0.f, ClampMax=1.f, UIMax=1.f))
	float SpawnProbability = 1.f;
};
