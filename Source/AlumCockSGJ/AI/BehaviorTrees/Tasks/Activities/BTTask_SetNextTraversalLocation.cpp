#include "BTTask_SetNextTraversalLocation.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetNextTraversalLocation::UBTTask_SetNextTraversalLocation()
{
	NodeName = "Set next traversal location";
	OutLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetNextTraversalLocation, OutLocationKey));
	StayInLocationDurationKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetNextTraversalLocation, StayInLocationDurationKey));
}

EBTNodeResult::Type UBTTask_SetNextTraversalLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNpcActivityInstanceBase* ActivityInstance = GetActivityInstance(OwnerComp);
	auto TraversalActivityInstance = Cast<ITraversalActivity>(ActivityInstance);
	if (!TraversalActivityInstance)
		return EBTNodeResult::Failed;

	FNextLocationData NextLocationData;
	bool bNextLocationFound = TraversalActivityInstance->FindNextLocation(OwnerComp.GetAIOwner(), NextLocationData);
	if (!bNextLocationFound)
		return EBTNodeResult::Failed;

	auto Blackboard = OwnerComp.GetBlackboardComponent();
	Blackboard->SetValueAsVector(OutLocationKey.SelectedKeyName, NextLocationData.NextLocation);
	if (!StayInLocationDurationKey.IsNone())
		Blackboard->SetValueAsFloat(StayInLocationDurationKey.SelectedKeyName, NextLocationData.RecommendedStayTime);	
	
	return EBTNodeResult::Succeeded;
}

FString UBTTask_SetNextTraversalLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Traversal Location BB: %s\nStay Duration BB: %s"),
		*OutLocationKey.SelectedKeyName.ToString(), *StayInLocationDurationKey.SelectedKeyName.ToString());
}
