#include "BTTask_ResetBlackboardValue.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ResetBlackboardValue::UBTTask_ResetBlackboardValue()
{
	NodeName = "Reset blackboard value";
}

EBTNodeResult::Type UBTTask_ResetBlackboardValue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!IsValid(Blackboard))
		return EBTNodeResult::Failed;

	Blackboard->ClearValue(KeyToReset.SelectedKeyName);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_ResetBlackboardValue::GetStaticDescription() const
{
	return FString::Printf(TEXT("Reset %s"), *KeyToReset.SelectedKeyName.ToString());
}
