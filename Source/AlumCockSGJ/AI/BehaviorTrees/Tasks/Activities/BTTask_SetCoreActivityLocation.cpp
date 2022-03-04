#include "BTTask_SetCoreActivityLocation.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetCoreActivityLocation::UBTTask_SetCoreActivityLocation()
{
	NodeName = "Set Core Activity Location";
	CoreActivityLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetCoreActivityLocation, CoreActivityLocationKey));
}

EBTNodeResult::Type UBTTask_SetCoreActivityLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto ActivityInstance = GetActivityInstance(OwnerComp);
	if (!ActivityInstance)
		return EBTNodeResult::Failed;
	
	auto CoreActivityLocation = ActivityInstance->GetCoreLocation();
	if (!CoreActivityLocation)
		return EBTNodeResult::Failed;
	
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(CoreActivityLocationKey.SelectedKeyName, CoreActivityLocation->GetActorLocation());
	return EBTNodeResult::Succeeded;
}
