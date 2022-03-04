#include "BTService_MaintainFollowingAlong.h"

#include "AIController.h"
#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/NpcFollowingActivity.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_MaintainFollowingAlong::UBTService_MaintainFollowingAlong(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NodeName = "Maintain following along";
	FollowedActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_MaintainFollowingAlong, FollowedActorKey), AActor::StaticClass());
	FollowedActorStoppedFlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_MaintainFollowingAlong, FollowedActorStoppedFlagKey));
}

void UBTService_MaintainFollowingAlong::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory,
	float DeltaSeconds)
{
	auto FollowedActor = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FollowedActorKey.SelectedKeyName));
	if (!FollowedActor)
		return;

	const float ActualPredictionTime = FMath::Min(Interval, PredictionTime);
	float CurrentFollowedActorSqSpeed = FollowedActor->GetCharacterMovement()->Velocity.SizeSquared2D();
	INpcFollowingActivity* FollowingActivity = Cast<INpcFollowingActivity>( GetActivityInstance(OwnerComp));
	if (!FollowingActivity)
		return;
	
	if (!FollowingActivity->IsFollowedActorStopped() && FMath::IsNearlyZero(CurrentFollowedActorSqSpeed))
	{
		FollowingActivity->SetFollowedActorStopped(true);
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(FollowedActorStoppedFlagKey.SelectedKeyName, true);
	}
	else
	{
		auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
		auto Npc = Cast<INpcCharacter>(Pawn);
		if (!Npc)
			return;
		
		if (FollowingActivity->IsFollowedActorStopped())
		{
			Npc->ResetDesiredSpeed();
			FollowingActivity->SetFollowedActorStopped(false);
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(FollowedActorStoppedFlagKey.SelectedKeyName, false);		
		}
		
		float Acceleration = FollowedActor->GetCharacterMovement()->GetCurrentAcceleration().Size();
		FVector ExpectedNextLocation = FollowedActor->GetActorLocation()
			+ FollowedActor->GetVelocity() * ActualPredictionTime + (Acceleration * ActualPredictionTime * ActualPredictionTime / 2.f);
		FollowingActivity->SetPredictedLocation(ExpectedNextLocation);
		// Npc->SetDesiredSpeed((ExpectedNextLocation - Pawn->GetActorLocation()))
	}
	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);		
}

FString UBTService_MaintainFollowingAlong::GetStaticDescription() const
{
	FString ServiceDescription = FString::Printf(TEXT("\nFollowed actor: %s\nFollowedActor stopped key: %s"),
		*FollowedActorKey.SelectedKeyName.ToString(), *FollowedActorStoppedFlagKey.SelectedKeyName.ToString());
	return Super::GetStaticDescription().Append(ServiceDescription);
}
