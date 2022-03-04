#include "BTService_KeepUpWithActor.h"

#include "AIController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "GameFramework/PawnMovementComponent.h"

UBTService_KeepUpWithActor::UBTService_KeepUpWithActor()
{
	NodeName = "Keep up with actor";
	KeepUpFlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_KeepUpWithActor, KeepUpFlagKey));
	FollowedActorStoppedKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_KeepUpWithActor, FollowedActorStoppedKey));
	ActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_KeepUpWithActor, ActorKey), APawn::StaticClass());
}

void UBTService_KeepUpWithActor::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	INpcCharacter* ControlledNpc = Cast<INpcCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!ControlledNpc)
		return;

	APawn* TargetActor = Cast<APawn>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(ActorKey.SelectedKeyName));
	FBTServiceMemory* Memory = (FBTServiceMemory*)NodeMemory;
	float DistSq = FVector::DistSquared(TargetActor->GetActorLocation(), Pawn->GetActorLocation());
	if (DistSq > KeepUpDistance * KeepUpDistance)
	{
		if (!Memory->bKeepingUp)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(KeepUpFlagKey.SelectedKeyName, true);
			Memory->bKeepingUp = true;
		}
		
		ControlledNpc->SetDesiredSpeed(FMath::Max( TargetActor->GetVelocity().Size() * KeepUpSpeedFactor, Pawn->GetMovementComponent()->GetMaxSpeed()));
	}
	else if (Memory->bKeepingUp)
	{
		Memory->bKeepingUp = false;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(KeepUpFlagKey.SelectedKeyName, false);
		ControlledNpc->ResetDesiredSpeed();
	}
	
	if (!Memory->bFollowedActorStoppedKey && TargetActor->GetVelocity().IsNearlyZero())
	{
		Memory->bFollowedActorStoppedKey = true;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FollowedActorStoppedKey.SelectedKeyName, true);
	}
	else if (Memory->bFollowedActorStoppedKey && !TargetActor->GetVelocity().IsNearlyZero())
	{
		Memory->bFollowedActorStoppedKey = false;
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FollowedActorStoppedKey.SelectedKeyName, false);
	}
}

FString UBTService_KeepUpWithActor::GetStaticDescription() const
{
	FString ServiceDescription = FString::Printf(TEXT("\nKeep up with %s on Distance: %.2f\nKeep Up flag: %s\nFollowed actor stopped flag: %s"),
		*ActorKey.SelectedKeyName.ToString(), KeepUpDistance, *KeepUpFlagKey.SelectedKeyName.ToString(), *FollowedActorStoppedKey.SelectedKeyName.ToString());
	return Super::GetStaticDescription().Append(ServiceDescription);
}
