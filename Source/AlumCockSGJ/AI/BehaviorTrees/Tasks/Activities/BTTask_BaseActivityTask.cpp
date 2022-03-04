#include "BTTask_BaseActivityTask.h"

#include "CommonConstants.h"
#include "AI/Interfaces/NpcController.h"
#include "AIController.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

FString UBTTask_BaseActivityTask::GetStaticDescription() const
{
	auto BlackboardKeys = GetBlackboardKeysForDescription();
	return Super::GetStaticDescription();
	// TODO don't be lazy and finish this
	if (BlackboardKeys.Num() <= 0)
		return Super::GetStaticDescription();
	
	FString KeyDesc("invalid");
	for (auto BBKey : BlackboardKeys)
	{
		if (BBKey.SelectedKeyType == UBlackboardKeyType_Object::StaticClass() ||
			BBKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
		{
			KeyDesc = BBKey.SelectedKeyName.ToString();
		}	
	}

	return FString::Printf(TEXT("%s: %s"), *Super::GetStaticDescription(), *KeyDesc);
}