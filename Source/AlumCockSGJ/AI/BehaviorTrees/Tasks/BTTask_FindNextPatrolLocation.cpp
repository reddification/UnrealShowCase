#include "BTTask_FindNextPatrolLocation.h"

#include "AIController.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Components/AIPatrolComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_FindNextPatrolLocation::UBTTask_FindNextPatrolLocation()
{
	NodeName = TEXT("Find Next Patrol Location");
}

EBTNodeResult::Type UBTTask_FindNextPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	APawn* Pawn = Controller->GetPawn();

	AAICharacter* AICharacter = Cast<AAICharacter>(Pawn);
	if (!IsValid(AICharacter))
		return EBTNodeResult::Failed;

	UAIPatrolComponent* PatrolComponent = AICharacter->GetAIPatrolComponent();
	if (PatrolComponent->CanPatrol())
	{
		bool bPatrolling = Blackboard->GetValueAsBool(IsPatrollingKey.SelectedKeyName);
		FVector NextWaypoint;
		if (bPatrolling)
		{
			NextWaypoint = PatrolComponent->SelectNextWaypoint();
		}
		else
		{
			NextWaypoint = PatrolComponent->SelectClosestWaypoint();
			Blackboard->SetValueAsBool(IsPatrollingKey.SelectedKeyName, true);
		}

		Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, NextWaypoint);
	}

	return EBTNodeResult::Succeeded;
}
