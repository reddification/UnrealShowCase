#include "BTTask_ProgressIteration.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"

UBTTask_ProgressIteration::UBTTask_ProgressIteration()
{
	NodeName = "Progress Iteration";
}

EBTNodeResult::Type UBTTask_ProgressIteration::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ActivityInstance = GetActivityInstance(OwnerComp);
	if (!ActivityInstance)
		return EBTNodeResult::Failed;

	ActivityInstance->ProgressIteration(IterationChange);
	return EBTNodeResult::Succeeded;
}

