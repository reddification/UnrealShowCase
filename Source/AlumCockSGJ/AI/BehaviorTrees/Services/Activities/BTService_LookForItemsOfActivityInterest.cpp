#include "BTService_LookForItemsOfActivityInterest.h"

#include "CommonConstants.h"
#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Interfaces/NpcController.h"
#include "AI/Interfaces/ObservationActivity.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_LookForItemsOfActivityInterest::UBTService_LookForItemsOfActivityInterest()
{
	NodeName = "Look for items of activity interest";	
}

void UBTService_LookForItemsOfActivityInterest::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	IObservationActivity* ActivityInstance = Cast<IObservationActivity>(GetActivityInstance(OwnerComp));
	if (!ActivityInstance)
		return;
	
	ActivityInstance->Observe(OwnerComp.GetAIOwner(), OwnerComp.GetBlackboardComponent());
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}


