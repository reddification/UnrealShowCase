#include "BTService_ProgressActivity.h"

#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Interfaces/NpcActivityManager.h"

UBTService_ProgressActivity::UBTService_ProgressActivity()
{
	NodeName = "Progress activity";
}

void UBTService_ProgressActivity::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto ActivityManager = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	if (ActivityManager)
		ActivityManager->GetRunningActivity()->Progress(DeltaSeconds);
}
