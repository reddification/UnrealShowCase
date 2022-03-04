#include "BTTask_ReportActivityEvent.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ReportActivityEvent::UBTTask_ReportActivityEvent()
{
	NodeName = "Report activity event";
}

EBTNodeResult::Type UBTTask_ReportActivityEvent::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Activity = GetActivityInstance(OwnerComp);
	if (!Activity)
		return EBTNodeResult::Failed;

	Activity->ReportActivityEvent(ActivityEvent);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_ReportActivityEvent::GetStaticDescription() const
{
	return UEnum::GetValueAsString(ActivityEvent);
}
