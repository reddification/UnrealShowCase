#pragma once

#include "CoreMinimal.h"
#include "AI/Data/NpcReactionTypes.h"
#include "UObject/Interface.h"
#include "FeelingsPropagator.generated.h"

UINTERFACE()
class UFeelingsPropagator : public UInterface
{
	GENERATED_BODY()
};


class ALUMCOCKSGJ_API IFeelingsPropagator
{
	GENERATED_BODY()

public:
	virtual FPropagatedFeelingsData GetAnxietyPropagation() { return FPropagatedFeelingsData(); }	
	virtual FPropagatedFeelingsData GetInterestPropagation() { return FPropagatedFeelingsData(); }	
};
