#include "BTTask_AdjustSpeed.h"

#include "AIController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_AdjustSpeed::UBTTask_AdjustSpeed()
{
	NodeName = "Adjust speed";
	TargetLocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_AdjustSpeed, TargetLocationKey));
}

EBTNodeResult::Type UBTTask_AdjustSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	auto Npc = Cast<INpcCharacter>(Pawn);
	if (!Npc)
		return EBTNodeResult::Failed;

	FVector TargetLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(TargetLocationKey.SelectedKeyName);
	float Distance = FVector::Distance(TargetLocation, Pawn->GetActorLocation());
	// float DesiredSpeed = 2 * Distance / PredictionTime - Pawn->GetVelocity().Size();
	Npc->SetDesiredSpeed(Distance / PredictionTime);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_AdjustSpeed::GetStaticDescription() const
{
	return FString::Printf(TEXT("Be at %s in %.2f s"), *TargetLocationKey.SelectedKeyName.ToString(), PredictionTime);
}
