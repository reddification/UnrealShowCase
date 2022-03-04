#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FeelingsProvider.generated.h"

UINTERFACE()
class UFeelingsProvider : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IFeelingsProvider
{
	GENERATED_BODY()

public:
	virtual TArray<FVector> GetInterestingLocations() { return TArray<FVector>(); }
	virtual TArray<FVector> GetAfraidLocations() { return TArray<FVector>(); }
};
