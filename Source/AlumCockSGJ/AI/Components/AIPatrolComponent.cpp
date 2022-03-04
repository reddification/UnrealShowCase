#include "AIPatrolComponent.h"
#include "Actors/Navigation/PatrolPath.h"

FVector UAIPatrolComponent::SelectClosestWaypoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	const TArray<FVector>& Waypoints = PatrolSettings.PatrolPath->GetWaypoints();
	const FTransform& PatrolPathTransform = PatrolSettings.PatrolPath->GetActorTransform();
	FVector ClosestWaypoint;
	float MinDistanceSq = FLT_MAX;
	for (auto i = 0; i < Waypoints.Num(); i++)
	{
		const FVector& Waypoint = Waypoints[i];
		const FVector& WorldWaypointLocation = PatrolPathTransform.TransformPosition(Waypoint);
		float CurrentSqDistance = (OwnerLocation - WorldWaypointLocation).SizeSquared();
		if (CurrentSqDistance < MinDistanceSq)
		{
			MinDistanceSq = CurrentSqDistance;
			ClosestWaypoint = WorldWaypointLocation;
			SelectedWaypointIndex = i;
		}
	}

	return ClosestWaypoint;
}

FVector UAIPatrolComponent::SelectNextWaypoint()
{
	const TArray<FVector>& Waypoints = PatrolSettings.PatrolPath->GetWaypoints();
	switch (PatrolSettings.PatrolType)
	{
		case EPatrolType::Circle:
			SelectedWaypointIndex = (SelectedWaypointIndex + 1) % Waypoints.Num();
		break;
		case EPatrolType::PingPong:
			if (Waypoints.Num() > 1)
			{
				if (SelectedWaypointIndex == Waypoints.Num() - 1)
				{
					PatrolSettings.PatrolPath->Reverse();
					SelectedWaypointIndex = 1;
				}
				else
				{
					SelectedWaypointIndex++;
				}
			}
		
		break;
	}
	
	const FTransform& PatrolPathTransform = PatrolSettings.PatrolPath->GetActorTransform();
	return PatrolPathTransform.TransformPosition(Waypoints[SelectedWaypointIndex]);
}

bool UAIPatrolComponent::CanPatrol() const
{
	return IsValid(PatrolSettings.PatrolPath) && PatrolSettings.PatrolPath->GetWaypoints().Num() > 0;
}

FVector UAIPatrolComponent::GetFocusPoint() const
{
	if (IsValid(PatrolSettings.PatrolPath))
	{
		return PatrolSettings.PatrolPath->GetFocusPoint();
	}

	return FVector::ZeroVector;
}

AActor* UAIPatrolComponent::GetFocusActor() const
{
	if (IsValid(PatrolSettings.PatrolPath))
	{
		return PatrolSettings.PatrolPath->GetFocusActor();
	}

	return nullptr;
}
