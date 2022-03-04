#include "BTTask_SetPatrolMode.h"
#include "AIController.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Components/Character/AICharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_SetPatrolMode::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!IsValid(Controller)) return EBTNodeResult::Failed;

	AAICharacter* BotCharacter = Cast<AAICharacter>(Controller->GetPawn());
	if (!IsValid(BotCharacter)) return EBTNodeResult::Failed;

	BotCharacter->GetAIMovementComponent()->SetIsPatrolling(bPatrolling);
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
