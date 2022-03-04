#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResetBlackboardValue.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_ResetBlackboardValue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ResetBlackboardValue();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector KeyToReset;
};
