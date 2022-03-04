#include "BTDecorator_SetFocus.h"

#include "AI/Controllers/AICitizenController.h"
#include "AI/Interfaces/NpcCharacter.h"

UBTDecorator_SetFocus::UBTDecorator_SetFocus()
{
	NodeName = "Set focus";
	bNotifyActivation = 1;
	bNotifyDeactivation = 1;
	ActorToFocusOnKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTDecorator_SetFocus, ActorToFocusOnKey), AActor::StaticClass());
}

void UBTDecorator_SetFocus::OnNodeActivation(FBehaviorTreeSearchData& SearchData)
{
	Super::OnNodeActivation(SearchData);
	AAIController* AIController = SearchData.OwnerComp.GetAIOwner();
	auto ActorToFocusOn = Cast<AActor>(SearchData.OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorToFocusOnKey.SelectedKeyName));
	if (ActorToFocusOn)
	{
		AIController->SetFocus(ActorToFocusOn);
		auto Npc = Cast<INpcCharacter>(AIController->GetPawn());
		if (Npc)
		{
			auto Memory = GetNodeMemory<FBTMemory_SetFocus>(SearchData);
			Memory->bInitialAiForcedStrafing = Npc->IsAiForcingStrafing();
			Npc->SetAiForcedStrafing(true);
		}
	}
}

void UBTDecorator_SetFocus::OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult)
{
	Super::OnNodeDeactivation(SearchData, NodeResult);
	AAIController* AIController = SearchData.OwnerComp.GetAIOwner();
	AIController->ClearFocus(EAIFocusPriority::Gameplay);
	auto Npc = Cast<INpcCharacter>(AIController->GetPawn());
	if (Npc)
	{
		auto Memory = GetNodeMemory<FBTMemory_SetFocus>(SearchData);
		Npc->SetAiForcedStrafing(Memory->bInitialAiForcedStrafing);
	}
}

FString UBTDecorator_SetFocus::GetStaticDescription() const
{
	return FString::Printf(TEXT("Focus target: %s"), *ActorToFocusOnKey.SelectedKeyName.ToString());
}
