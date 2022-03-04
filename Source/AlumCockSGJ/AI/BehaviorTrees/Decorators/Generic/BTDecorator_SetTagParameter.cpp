#include "BTDecorator_SetTagParameter.h"

#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_SetTagParameter::UBTDecorator_SetTagParameter()
{
	NodeName = "Set tag parameter";
	bNotifyActivation = 1;
	bNotifyDeactivation = 1;
	OutParameterKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_SetTagParameter, OutParameterKey));
}

void UBTDecorator_SetTagParameter::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	if (!GameplayTag.IsValid())
		return;

	FSetTagParameterMemory* Memory = GetNodeMemory<FSetTagParameterMemory>(SearchData);
	auto Blackboard = SearchData.OwnerComp.GetBlackboardComponent();
	Memory->PreviousTag = Blackboard->GetValueAsName(OutParameterKey.SelectedKeyName);
	Blackboard->SetValueAsName(OutParameterKey.SelectedKeyName, GameplayTag.GetTagName());
}

void UBTDecorator_SetTagParameter::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
	EBTNodeResult::Type NodeResult)
{
	if (!GameplayTag.IsValid() || OutParameterKey.SelectedKeyName.IsNone())
		return;

	FSetTagParameterMemory* Memory = GetNodeMemory<FSetTagParameterMemory>(SearchData);
	auto Blackboard = SearchData.OwnerComp.GetBlackboardComponent();
	Blackboard->SetValueAsName(OutParameterKey.SelectedKeyName, Memory->PreviousTag);
}

FString UBTDecorator_SetTagParameter::GetStaticDescription() const
{
	return FString::Printf(TEXT("Set %s = %s"), *OutParameterKey.SelectedKeyName.ToString(), *GameplayTag.GetTagName().ToString());
}

uint16 UBTDecorator_SetTagParameter::GetInstanceMemorySize() const
{
	return sizeof(FSetTagParameterMemory);
}
