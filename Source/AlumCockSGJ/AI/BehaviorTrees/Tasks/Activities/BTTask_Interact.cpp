#include "BTTask_Interact.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/InteractionActivity.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_Interact::UBTTask_Interact()
{
	NodeName = "Interact";
	ActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_Interact, ActorKey), AActor::StaticClass());
	bCreateNodeInstance = 1;
	// TODO maybe move outside of constructor
	OnInteractionCompleted.BindUObject(this, &UBTTask_Interact::OnTaskCompleted);
}

EBTNodeResult::Type UBTTask_Interact::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UNpcActivityInstanceBase* ActivityInstance = GetActivityInstance(OwnerComp);
	IInteractionActivity* InteractionActivity = Cast<IInteractionActivity>(ActivityInstance);
	if (!InteractionActivity)
		return EBTNodeResult::Failed;
	
	AActor* InteractionActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorKey.SelectedKeyName));
	if (!InteractionActor)
		return EBTNodeResult::Failed;
	
	MyOwnerComp = &OwnerComp;
	bool bActionStarted = bStartInteract
		? InteractionActivity->StartInteracting(OwnerComp.GetAIOwner(), InteractionActor, &OnInteractionCompleted, bResetInteractionActorOnFail)
		: InteractionActivity->StopInteracting(OwnerComp.GetAIOwner(), &OnInteractionCompleted, bResetInteractionActorOnFail);
	
	return bActionStarted
		? EBTNodeResult::InProgress
		: EBTNodeResult::Failed;
}

FString UBTTask_Interact::GetStaticDescription() const
{
	return FString::Printf(TEXT("Interact with %s\nReset on interaction failed: %hs"),
		*ActorKey.SelectedKeyName.ToString(), (bResetInteractionActorOnFail ? "true" : "false"));
}

void UBTTask_Interact::OnTaskCompleted(ENpcActivityLatentActionState State)
{
	EBTNodeResult::Type Result;
	switch (State)
	{
		case ENpcActivityLatentActionState::Completed:
		case ENpcActivityLatentActionState::Started:
			Result = EBTNodeResult::Succeeded;
		break;
		case ENpcActivityLatentActionState::Aborted:
			Result = EBTNodeResult::Aborted;
		break;
		case ENpcActivityLatentActionState::Failed:
		default:
			Result = EBTNodeResult::Failed;
		break;
	}

	FinishLatentTask(*MyOwnerComp, Result);
}
