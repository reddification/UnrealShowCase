#include "BTTask_RunActivityEQSQuery.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"

UBTTask_RunActivityEQSQuery::UBTTask_RunActivityEQSQuery(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Run Activity Query";
	BlackboardKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_RunActivityEQSQuery, BlackboardKey));
	BlackboardKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_RunActivityEQSQuery, BlackboardKey), AActor::StaticClass());
}

EBTNodeResult::Type UBTTask_RunActivityEQSQuery::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (!ActivityQueryTag.IsValid())
		return EBTNodeResult::Failed;

	auto ActivityInstance = GetActivityInstance(OwnerComp);
	auto ActivityQueryTemplate = ActivityInstance->GetQueryTemplate(ActivityQueryTag);
	if (!ActivityQueryTemplate)
		return EBTNodeResult::Failed;
	
	EQSRequest.QueryTemplate = ActivityQueryTemplate;
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}

FString UBTTask_RunActivityEQSQuery::GetStaticDescription() const
{
	return Super::GetStaticDescription().Append(FString::Printf(TEXT("\nActivity Query Tag: %s"), *ActivityQueryTag.ToString()));
}
