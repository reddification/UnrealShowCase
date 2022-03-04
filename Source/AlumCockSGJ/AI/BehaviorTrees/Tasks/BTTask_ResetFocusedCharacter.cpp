#include "BTTask_ResetFocusedCharacter.h"

#include "AI/Controllers/AIScissorsController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_ResetFocusedCharacter::UBTTask_ResetFocusedCharacter()
{
	NodeName = "Reset focused character";
}

EBTNodeResult::Type UBTTask_ResetFocusedCharacter::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AIController = Cast<AAIScissorsController>(OwnerComp.GetAIOwner());
	if (IsValid(AIController))
	{
		AIController->ResetFocusedCharacter();
	}

	if (!CharacterBlackboardKey.IsNone())
	{
		auto Blackboard = OwnerComp.GetBlackboardComponent();
		if (IsValid(Blackboard))
		{
			Blackboard->ClearValue(CharacterBlackboardKey.SelectedKeyName);
		}
	}
	
	return EBTNodeResult::Succeeded;
}
