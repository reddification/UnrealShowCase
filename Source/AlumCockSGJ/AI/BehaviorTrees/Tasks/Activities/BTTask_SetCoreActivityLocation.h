#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetCoreActivityLocation.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_SetCoreActivityLocation : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_SetCoreActivityLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector CoreActivityLocationKey;
};
