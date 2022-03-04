#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ApproachTarget.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_ApproachTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ApproachTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Radius = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector LocationKey;
};
