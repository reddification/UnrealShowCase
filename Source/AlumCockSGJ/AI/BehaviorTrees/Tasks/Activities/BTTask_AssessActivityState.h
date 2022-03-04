#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AssessActivityState.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_AssessActivityState : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_AssessActivityState();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
