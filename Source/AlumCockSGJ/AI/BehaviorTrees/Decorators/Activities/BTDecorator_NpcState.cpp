#include "BTDecorator_NpcState.h"

#include "AIController.h"
#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/StatefulActivity.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_NpcState::UBTDecorator_NpcState()
{
	NodeName = "Set NPC State";
	bNotifyActivation = 1;
	bNotifyDeactivation = 1;
	NpcStateParameterKey.AllowNoneAsValue(true);
	NpcStateParameterKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_NpcState, NpcStateParameterKey));
}

void UBTDecorator_NpcState::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
	auto StatefulCharacter = Cast<INpcCharacter>(SearchData.OwnerComp.GetAIOwner()->GetPawn());
	if (!StatefulCharacter)
		return;

	FGameplayTag ActualStateTag = GetActualTag(SearchData.OwnerComp);
	FNpcStateMemory* NodeMemory = GetNodeMemory<FNpcStateMemory>(SearchData);
	NodeMemory->InitialTag = StatefulCharacter->GetState();
	StatefulCharacter->TrySetState(ActualStateTag);
}

FGameplayTag UBTDecorator_NpcState::GetActualTag(UBehaviorTreeComponent& OwnerComp) const
{
	if (bFromActivity)
	{
		UNpcActivityInstanceBase* ActivityInstance = GetActivityInstance(OwnerComp);
		IStatefulActivity* StatefulActivity = Cast<IStatefulActivity>(ActivityInstance);
		return StatefulActivity ? StatefulActivity->GetStateTag() : FGameplayTag::EmptyTag;
	}
	else if (NpcStateParameterKey.IsSet())
	{
		FName GameplayTagName = OwnerComp.GetBlackboardComponent()->GetValueAsName(NpcStateParameterKey.SelectedKeyName);
		return FGameplayTag::RequestGameplayTag(GameplayTagName);
	}
	else
	{
		return StateTag;
	}
}

void UBTDecorator_NpcState::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	Super::OnNodeDeactivation(SearchData, NodeResult);
	auto StatefulCharacter = Cast<INpcCharacter>(SearchData.OwnerComp.GetAIOwner()->GetPawn());
	if (!StatefulCharacter)
		return;

	FNpcStateMemory* NodeMemory = GetNodeMemory<FNpcStateMemory>(SearchData);
	StatefulCharacter->TrySetState(NodeMemory->InitialTag);
}

FString UBTDecorator_NpcState::GetStaticDescription() const
{
	if (bFromActivity)
		return "NPC State from activity";
	
	if (NpcStateParameterKey.IsSet())
		return FString::Printf(TEXT("Parametrized state: %s"), *NpcStateParameterKey.SelectedKeyName.ToString());

	return StateTag.IsValid()
		? FString::Printf(TEXT("Branch state: %s"), *StateTag.ToString())
		: FString::Printf(TEXT("Branch state not set"));
}