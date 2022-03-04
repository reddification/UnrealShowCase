#include "BTTask_RunBehaviorDynamicStateful.h"

#include "AIController.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Controllers/AICitizenController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Data/LogTypes.h"

UBTTask_RunBehaviorDynamicStateful::UBTTask_RunBehaviorDynamicStateful(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	NodeName = "Run Behavior Dynamic Stateful";
}

EBTNodeResult::Type UBTTask_RunBehaviorDynamicStateful::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
                                                                    uint8* NodeMemory)
{
	// UE_LOG(LogNpcActivity, Verbose, TEXT("Starting DBT with tag %s at time %f"), *InjectionTag.ToString(), OwnerComp.GetWorld()->GetTimeSeconds());
	
	auto Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard)
		return EBTNodeResult::Failed;

	auto AIController = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	if (!AIController)
		return EBTNodeResult::Failed;

	auto NewBTTag = InjectionTag.ToString();
	auto NpcActivityInstance = Cast<UNpcActivityInstanceBase>(AIController->GetRunningActivity());
	if (!NpcActivityInstance)
		return EBTNodeResult::Failed;
	
	NpcActivityInstance->RestoreBlackboardState(Blackboard);
	NpcActivityInstance->Resume();
	return Result;
	// return Super::ExecuteTask(OwnerComp, NodeMemory);
}

void UBTTask_RunBehaviorDynamicStateful::OnSubtreeDeactivated(UBehaviorTreeComponent& OwnerComp,
	EBTNodeResult::Type NodeResult)
{
	// UE_LOG(LogNpcActivity, Verbose, TEXT("Deactivating DBT with tag %s at time %f"), *InjectionTag.ToString(), OwnerComp.GetWorld()->GetTimeSeconds());
	
	auto ActivityManager = Cast<INpcActivityManager>(OwnerComp.GetAIOwner());
	if (!ActivityManager)
		return;

	auto RunningActivity = ActivityManager->GetRunningActivity();
	if (RunningActivity)
	{
		bool bInteractionInterruptRequired = ActivityManager->IsInteractionInterruptionRequired();
		RunningActivity->Suspend(OwnerComp.GetAIOwner(), bInteractionInterruptRequired);
	}
	
	Super::OnSubtreeDeactivated(OwnerComp, NodeResult);
}