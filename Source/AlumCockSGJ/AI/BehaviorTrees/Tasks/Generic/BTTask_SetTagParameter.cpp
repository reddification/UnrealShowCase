#include "BTTask_SetTagParameter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetTagParameter::UBTTask_SetTagParameter()
{
	NodeName = "Set tag parameter";
	OutParameterKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_SetTagParameter, OutParameterKey));
}

EBTNodeResult::Type UBTTask_SetTagParameter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!GameplayTag.IsValid())
		return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsName(OutParameterKey.SelectedKeyName, GameplayTag.GetTagName());
	return EBTNodeResult::Succeeded;
}

FString UBTTask_SetTagParameter::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set %s = %s"), *OutParameterKey.SelectedKeyName.ToString(), *GameplayTag.GetTagName().ToString());
}
