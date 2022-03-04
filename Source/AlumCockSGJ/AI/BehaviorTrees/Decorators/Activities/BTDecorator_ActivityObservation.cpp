#include "BTDecorator_ActivityObservation.h"

#include "CommonConstants.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcController.h"

UBTDecorator_ActivityObservation::UBTDecorator_ActivityObservation()
{
	NodeName = "Activity observation";
	bNotifyActivation = 1;
	bNotifyDeactivation = 1;
}

void UBTDecorator_ActivityObservation::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	auto ActivityInstance = GetActivityInstance(SearchData.OwnerComp);
	if (!ActivityInstance)
		return;

	ActivityInstance->StartObserving(SearchData.OwnerComp.GetAIOwner());
}

void UBTDecorator_ActivityObservation::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData,
	EBTNodeResult::Type NodeResult)
{
	auto ActivityInstance = GetActivityInstance(SearchData.OwnerComp);
	if (!ActivityInstance)
		return;

	ActivityInstance->StopObserving(SearchData.OwnerComp.GetAIOwner());
}

UNpcActivityInstanceBase* UBTDecorator_ActivityObservation::GetActivityInstance(UBehaviorTreeComponent& OwnerComp)
{
	auto AIController = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	if (!AIController)
		return nullptr;

	return AIController->GetRunningActivity();
}