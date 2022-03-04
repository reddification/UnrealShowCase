#include "BTService_SeeLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_SeeLocation::UBTService_SeeLocation()
{
	NodeName = "Can see location";
	LocationKey.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SeeLocation, LocationKey));
	ObservationSuccessFlagKey.AddBoolFilter(this, GET_MEMBER_NAME_CHECKED(UBTService_SeeLocation, ObservationSuccessFlagKey));
}

void UBTService_SeeLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	FVector Location = OwnerComp.GetBlackboardComponent()->GetValueAsVector(LocationKey.SelectedKeyName) + FVector::UpVector * 100.f;
	auto Pawn = OwnerComp.GetAIOwner()->GetPawn();
	FVector PawnLocation = Pawn->GetActorLocation();

	// TODO mind the performance
	float PathDistance = -1.f;
	auto FindPathLengthResult = UNavigationSystemV1::GetPathLength(OwnerComp.GetWorld(), PawnLocation, Location,PathDistance);
	if (PathDistance > RecognizeDistance)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ObservationSuccessFlagKey.SelectedKeyName, false);
		return;
	}
	
	if (FindPathLengthResult != ENavigationQueryResult::Success)
	{
		float SquaredDistance = FVector::DistSquared(Location, PawnLocation);
		if (SquaredDistance > RecognizeDistance * RecognizeDistance)
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsBool(ObservationSuccessFlagKey.SelectedKeyName, false);
			return;
		}
	}

	FRotator ViewDirection = Pawn->GetControlRotation();
	FVector ViewDirectionV = ViewDirection.Vector();
	FVector ViewFV = Pawn->GetActorForwardVector();
	
	float DotProduct = FVector::DotProduct((Location - PawnLocation).GetSafeNormal(), ViewFV);
	if (DotProduct < RecognizeTraceDotProductThreshold)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool(ObservationSuccessFlagKey.SelectedKeyName, false);
		return;
	}
	
	FHitResult TraceResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(OwnerComp.GetAIOwner()->GetPawn());
	bool bHit = OwnerComp.GetWorld()->LineTraceSingleByChannel(TraceResult, PawnLocation, Location, ECC_Visibility, CollisionQueryParams);
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(ObservationSuccessFlagKey.SelectedKeyName, !bHit);
}

FString UBTService_SeeLocation::GetStaticDescription() const
{
	return FString::Printf(TEXT("%s\nLocation: %s"), *Super::GetStaticDescription(), *LocationKey.SelectedKeyName.ToString());
}
