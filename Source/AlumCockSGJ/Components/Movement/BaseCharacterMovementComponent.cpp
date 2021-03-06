#include "BaseCharacterMovementComponent.h"

#include "GameFramework/Character.h"

void UBaseCharacterMovementComponent::SetForcedWalkSpeed(float ForcedWalkSpeed)
{
	if (bForcedSpeed && ForcedWalkSpeed < 0.f)
	{
		bForcedSpeed = false;
		MaxWalkSpeed = PreForcedSpeed;
	}
	else
	{
		if (!bForcedSpeed)
		{
			bForcedSpeed = true;
			PreForcedSpeed = MaxWalkSpeed;
		}

		MaxWalkSpeed = ForcedWalkSpeed;
	}
}

void UBaseCharacterMovementComponent::SetDesiredRotation(const FRotator& Rotator)
{
	bForceRotation = true;
	ForceTargetRotation = Rotator;
}

bool UBaseCharacterMovementComponent::CanApplyCustomRotation()
{
	return !CharacterOwner->bUseControllerRotationYaw;
}

void UBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if (bForceRotation && CanApplyCustomRotation())
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			if (IsMovingOnGround())
			{
				UE_LOG(LogTemp, Log, TEXT("DR: %s, TR: %s"), *DesiredRotation.ToString(), *ForceTargetRotation.ToString());
			}
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation,  false );
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator; 
			bForceRotation = false;
			if (DesiredRotationReachedEvent.IsBound())
				DesiredRotationReachedEvent.Broadcast();
		}
	
		return;
	}

	if (IgnorePhysicsRotation())
		return;

	Super::PhysicsRotation(DeltaTime);
}
