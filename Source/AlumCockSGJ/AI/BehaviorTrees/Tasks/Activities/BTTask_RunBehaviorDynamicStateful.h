#pragma once

#include "CoreMinimal.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/Tasks/BTTask_RunBehaviorDynamic.h"
#include "BTTask_RunBehaviorDynamicStateful.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_RunBehaviorDynamicStateful : public UBTTask_RunBehaviorDynamic
{
	GENERATED_BODY()
	
public:
	UBTTask_RunBehaviorDynamicStateful(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void OnSubtreeDeactivated(UBehaviorTreeComponent& OwnerComp, EBTNodeResult::Type NodeResult) override;
};
