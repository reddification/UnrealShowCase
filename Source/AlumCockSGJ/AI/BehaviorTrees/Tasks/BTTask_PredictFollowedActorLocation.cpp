#include "BTTask_PredictFollowedActorLocation.h"

#include "AI/ActivityInstances/ActivityInstancesHelper.h"
#include "AI/Interfaces/NpcFollowingActivity.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTTask_PredictFollowedActorLocation::UBTTask_PredictFollowedActorLocation()
{
	NodeName = "Predict followed actor location";
	FollowedActorKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_PredictFollowedActorLocation, FollowedActorKey), APawn::StaticClass());
}

EBTNodeResult::Type UBTTask_PredictFollowedActorLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory)
{
	auto NpcFollowingActivity = Cast<INpcFollowingActivity>(GetActivityInstance(OwnerComp));
	if (!NpcFollowingActivity)
		return EBTNodeResult::Failed;

	auto FollowedActor = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(FollowedActorKey.SelectedKeyName));
	if (!FollowedActor)
		return EBTNodeResult::Failed;

	// float Acceleration = FollowedActor->GetCharacterMovement()->GetCurrentAcceleration().Size();
	FVector ActorLocation = FollowedActor->GetActorLocation();
	FVector ActorVelocity = FollowedActor->GetVelocity();
	// FVector NormalizedVelocity = ActorVelocity.GetSafeNormal();
	// float CurrentSpeed = ActorVelocity.Size();
	// FVector ExpectedNextLocation = ActorLocation + NormalizedVelocity * (CurrentSpeed * PredictionTime + (Acceleration * PredictionTime * PredictionTime / 2.f));
	FVector ExpectedNextLocation = ActorLocation + ActorVelocity * PredictionTime;
	NpcFollowingActivity->SetPredictedLocation(ExpectedNextLocation);
	NpcFollowingActivity->SetFollowedActor(FollowedActor);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_PredictFollowedActorLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("Followed actor: %s\nPrediction Time: %.2f"), *FollowedActorKey.SelectedKeyName.ToString(), PredictionTime);
}
