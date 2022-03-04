#include "Actors/Navigation/PatrolPath.h"

void APatrolPath::Reverse()
{
	Algo::Reverse(Waypoints);
}

FVector APatrolPath::GetFocusPoint() const
{
	return GetTransform().TransformPosition(ViewCenter);
}

FVector APatrolPath::GetGuardLocation() const
{
	return GetTransform().TransformPosition(GuardLocation);
}
