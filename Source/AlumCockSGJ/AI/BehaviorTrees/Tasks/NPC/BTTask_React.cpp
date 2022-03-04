#include "BTTask_React.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/ReactionActivity.h"
#include "Subsystems/NpcSubsystem.h"

UBTTask_React::UBTTask_React()
{
	NodeName = "Express reaction";
	bNotifyTaskFinished = 1;
	bNotifyTick = 1;
	ParameterReactionTypeTag.AllowNoneAsValue(false);
	ParameterReactionTypeTag.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_React, ParameterReactionTypeTag));
	OutRecommendedStayTimeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_React, OutRecommendedStayTimeKey));
}

void UBTTask_React::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (ReactTaskType == EAiReactTaskType::Locking)
	{
		FNpcReactTaskMemory* Memory = (FNpcReactTaskMemory*)NodeMemory;
		Memory->TimeLeft -= DeltaSeconds;
		if (Memory->TimeLeft <= 0)
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type UBTTask_React::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto NpcSubsystem = OwnerComp.GetWorld()->GetSubsystem<UNpcSubsystem>();
	if (!NpcSubsystem)
		return EBTNodeResult::Failed;

	auto NpcCharacter = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NpcCharacter)
		return EBTNodeResult::Failed;

	FNpcReactTaskMemory* Memory = (FNpcReactTaskMemory*)NodeMemory;
	const FGameplayTag& ActualTag = GetActualTag(OwnerComp);
	if (!ActualTag.IsValid())
		return EBTNodeResult::Failed;

	FNpcReactResult Result = NpcSubsystem->TryReact(NpcCharacter, ActualTag);
	if (!Result.bStarted)
		return EBTNodeResult::Failed;
	
	Memory->RunningTag = ActualTag;
	Memory->bStarted = Result.bStarted;
	Memory->ReactionIndex = Result.ReactionIndex;
	Memory->TimeLeft = Result.ExpectedDuration;
	
	OwnerComp.GetBlackboardComponent()->SetValueAsFloat(OutRecommendedStayTimeKey.SelectedKeyName, Result.ExpectedDuration);
	return ReactTaskType == EAiReactTaskType::Immediate ? EBTNodeResult::Succeeded : EBTNodeResult::InProgress;
}

FString UBTTask_React::GetStaticDescription() const
{
	FString Description = bTagFromActivity
		? "Reaction tag from activity"
		: ReactionTypeTag.IsValid()
			? ReactionTypeTag.ToString()
			: ParameterReactionTypeTag.IsSet()
				? FString::Printf(TEXT("Parameter: %s"), *ParameterReactionTypeTag.SelectedKeyName.ToString())
				: "None";

	Description = Description.Append(FString::Printf(TEXT("\n%s"), *UEnum::GetValueAsString(ReactTaskType)));
	return Description;
}

uint16 UBTTask_React::GetInstanceMemorySize() const
{
	return sizeof(FNpcReactTaskMemory);
}

void UBTTask_React::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
	FNpcReactTaskMemory* Memory = (FNpcReactTaskMemory*)NodeMemory;
	if (!Memory->bStarted || ReactTaskType == EAiReactTaskType::Immediate)
		return;
	
	auto NpcSubsystem = OwnerComp.GetWorld()->GetSubsystem<UNpcSubsystem>();
	if (!NpcSubsystem)
		return;

	auto NpcCharacter = Cast<ABaseCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!NpcCharacter)
		return;

	NpcSubsystem->TryFinishReaction(NpcCharacter, Memory->RunningTag, Memory->ReactionIndex);
}

FGameplayTag UBTTask_React::GetActualTag(UBehaviorTreeComponent& OwnerComp) const
{
	if (bTagFromActivity)
	{
		UNpcActivityInstanceBase* ActivityInstance = GetActivityInstance(OwnerComp);
		IReactionActivity* ReactionActivity = Cast<IReactionActivity>(ActivityInstance);
		return ReactionActivity ? ReactionActivity->GetReactionTag() : FGameplayTag::EmptyTag;
	}
	else if (ReactionTypeTag.IsValid())
	{
		return ReactionTypeTag;
	}
	else if (!ParameterReactionTypeTag.SelectedKeyName.IsNone())
	{
		FName GameplayTagName = OwnerComp.GetBlackboardComponent()->GetValueAsName(ParameterReactionTypeTag.SelectedKeyName);
		return FGameplayTag::RequestGameplayTag(GameplayTagName, false);
	}
	
	else return FGameplayTag::EmptyTag;
}
