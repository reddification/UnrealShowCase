#include "BTDecorator_IsTargetAlive.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/Killable.h"

UBTDecorator_IsTargetAlive::UBTDecorator_IsTargetAlive()
{
	NodeName = "Is target alive";
}

bool UBTDecorator_IsTargetAlive::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard))
		return false;

	IKillable* KillableActor = Cast<IKillable>(Blackboard->GetValueAsObject(ActorKey.SelectedKeyName));
	return KillableActor
		? KillableActor->IsAlive()
		: false;
}
