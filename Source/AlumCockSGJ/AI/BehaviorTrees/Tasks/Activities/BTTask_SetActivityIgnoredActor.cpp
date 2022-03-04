#include "BTTask_SetActivityIgnoredActor.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"

UBTTask_SetActivityIgnoredActor::UBTTask_SetActivityIgnoredActor()
{
	NodeName = "Set activity ignored actor";
	ActorToIgnore.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetActivityIgnoredActor, ActorToIgnore), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_SetActivityIgnoredActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ActivityInstance = GetActivityInstance(OwnerComp);
	if (!ActivityInstance)
		return EBTNodeResult::Failed;

	auto IgnoredActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorToIgnore.SelectedKeyName));
	ActivityInstance->SetActorToIgnore(IgnoredActor);
	
	return EBTNodeResult::Succeeded;
}
