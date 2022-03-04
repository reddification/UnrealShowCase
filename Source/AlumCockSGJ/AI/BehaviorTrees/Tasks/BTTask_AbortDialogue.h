#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AbortDialogue.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_AbortDialogue : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AbortDialogue();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
