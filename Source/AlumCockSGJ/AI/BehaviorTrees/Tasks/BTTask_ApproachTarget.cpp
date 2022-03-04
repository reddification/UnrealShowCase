#include "BTTask_ApproachTarget.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ApproachTarget::UBTTask_ApproachTarget()
{
	NodeName = TEXT("Approach target");
}

// piece of shit. bot always loses character because it goes to a random point which is out of characters sight
EBTNodeResult::Type UBTTask_ApproachTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (!IsValid(Controller) || !IsValid(Blackboard))
		return EBTNodeResult::Failed;
	
	APawn* Pawn = Controller->GetPawn();
	if (!IsValid(Pawn))
		return EBTNodeResult::Failed;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn);
	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if (!IsValid(TargetActor))
		return EBTNodeResult::Failed;

	FNavLocation NavLocation;
	bool bFound = NavSys->GetRandomReachablePointInRadius(TargetActor->GetActorLocation(), Radius, NavLocation);
	if (!bFound)
		return EBTNodeResult::Failed;

	Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, NavLocation.Location);
	return EBTNodeResult::Succeeded;
}
