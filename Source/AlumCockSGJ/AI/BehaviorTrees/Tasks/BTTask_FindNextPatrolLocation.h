#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindNextPatrolLocation.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_FindNextPatrolLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindNextPatrolLocation();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector IsPatrollingKey;
};
