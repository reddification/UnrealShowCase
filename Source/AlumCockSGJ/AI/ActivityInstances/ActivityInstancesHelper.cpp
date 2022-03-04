#include "ActivityInstancesHelper.h"

#include "AIController.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UNpcActivityInstanceBase* GetActivityInstance(const UBehaviorTreeComponent& OwnerComp)
{
	INpcActivityManager* AIController = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	if (!AIController)
		return nullptr;

	return AIController->GetRunningActivity();
}
