#pragma once

#include "CoreMinimal.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BaseActivityTask.generated.h"

UCLASS(Abstract)
class ALUMCOCKSGJ_API UBTTask_BaseActivityTask : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual FString GetStaticDescription() const override;
	
protected:
	virtual TArray<FBlackboardKeySelector> GetBlackboardKeysForDescription() const { return TArray<FBlackboardKeySelector>(); }
};
