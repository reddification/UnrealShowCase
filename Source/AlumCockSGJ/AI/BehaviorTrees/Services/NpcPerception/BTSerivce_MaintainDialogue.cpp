#include "BTSerivce_MaintainDialogue.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTSerivce_MaintainDialogue::UBTSerivce_MaintainDialogue()
{
	NodeName = "Maintain dialogue";
	CallInterlocutorFlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTSerivce_MaintainDialogue, CallInterlocutorFlagKey));
	FinishDialogueFlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTSerivce_MaintainDialogue, FinishDialogueFlagKey));
	InterlocutorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTSerivce_MaintainDialogue, InterlocutorKey), AActor::StaticClass());
	bNotifyBecomeRelevant = 1;
}

void UBTSerivce_MaintainDialogue::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(CallInterlocutorFlagKey.SelectedKeyName, false);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(FinishDialogueFlagKey.SelectedKeyName, false);
}

void UBTSerivce_MaintainDialogue::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto Blackboard = OwnerComp.GetBlackboardComponent();
	auto Interlocutor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(InterlocutorKey.SelectedKeyName));
	float SqDistance = FVector::DistSquared(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), Interlocutor->GetActorLocation());
	if (SqDistance > MaintainDialogueDistance * MaintainDialogueDistance)
		 Blackboard->SetValueAsBool(FinishDialogueFlagKey.SelectedKeyName, true);
	else if (SqDistance > CallInterlocutorAtRange * CallInterlocutorAtRange)
		Blackboard->SetValueAsBool(CallInterlocutorFlagKey.SelectedKeyName, true);
}

FString UBTSerivce_MaintainDialogue::GetStaticDescription() const
{
	return FString::Printf(
		TEXT("Maintain dialogue range: %.2f\nCall Interlocutor range: %.2f\nInterlocutor BB: %s\nCall Interlocutor BB: %s\nFinish dialogue BB: %s"),
		MaintainDialogueDistance, CallInterlocutorAtRange, *InterlocutorKey.SelectedKeyName.ToString(),
		*CallInterlocutorFlagKey.SelectedKeyName.ToString(), *FinishDialogueFlagKey.SelectedKeyName.ToString());
}
