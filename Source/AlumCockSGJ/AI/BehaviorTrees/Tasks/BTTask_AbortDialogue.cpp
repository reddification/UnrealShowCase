#include "BTTask_AbortDialogue.h"

#include "AIController.h"
#include "Subsystems/DialogueSubsystem.h"

UBTTask_AbortDialogue::UBTTask_AbortDialogue()
{
	NodeName = "Abort dialogue (if any)";
}

EBTNodeResult::Type UBTTask_AbortDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto AIController = OwnerComp.GetAIOwner();
	auto DialogueSubsystem = AIController->GetGameInstance()->GetSubsystem<UDialogueSubsystem>();
	DialogueSubsystem->InterruptDialogue(Cast<ABaseCharacter>(AIController->GetPawn()));
	return EBTNodeResult::Succeeded;
}
