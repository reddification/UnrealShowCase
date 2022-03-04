
#include "AICharacterMovementComponent.h"

float UAICharacterMovementComponent::GetMaxSpeed() const
{
	if (bPursuing) return PursueSpeed;
	if (bPatrolling) return PatrolSpeed;
	return Super::GetMaxSpeed();
}
