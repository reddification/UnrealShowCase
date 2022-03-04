#pragma once

#include "CommonWrappers.generated.h"

USTRUCT(BlueprintType)
struct FMontagesWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<UAnimMontage*> Montages;
};
