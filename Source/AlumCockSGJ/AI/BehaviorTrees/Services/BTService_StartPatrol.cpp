#include "BTService_StartPatrol.h"

#include "AIController.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Components/Character/AICharacterMovementComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_StartPatrol::UBTService_StartPatrol()
{
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
}

void UBTService_StartPatrol::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	SetIsPatrolling(OwnerComp, true);
}

void UBTService_StartPatrol::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	SetIsPatrolling(OwnerComp, false);
}

void UBTService_StartPatrol::SetIsPatrolling(UBehaviorTreeComponent& OwnerComp, bool bState)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Controller) || !IsValid(Blackboard)) return;

	AAICharacter* BotCharacter = Cast<AAICharacter>(Controller->GetPawn());
	if (!IsValid(BotCharacter)) return;

	BotCharacter->GetAIMovementComponent()->SetIsPatrolling(bState);
}
