#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "BTTask_ProgressIteration.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_ProgressIteration : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_ProgressIteration();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int IterationChange = 1;
};
