#include "BTService_Restore.h"

#include "AIController.h"
#include "AI/Interfaces/NpcActivityManager.h"

UBTService_Restore::UBTService_Restore()
{
	NodeName = "Restore utility and energy";
}

void UBTService_Restore::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto ActivityController = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	ActivityController->Restore(DeltaSeconds);
}

