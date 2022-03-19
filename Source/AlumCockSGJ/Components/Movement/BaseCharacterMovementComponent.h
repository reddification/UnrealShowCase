#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FDesiredRotationReachedEvent)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void SetForcedWalkSpeed(float ForcedWalkSpeed);
	void SetDesiredRotation(const FRotator& Rotator);

	virtual void PhysicsRotation(float DeltaTime) override;
	void UnsetDesiredRotation() { bForceRotation = false; }
	bool IsForcingRotation() const { return bForceRotation; }
	mutable FDesiredRotationReachedEvent DesiredRotationReachedEvent;

protected:
	virtual bool IgnorePhysicsRotation() { return false; }
	bool CanApplyCustomRotation();

	bool bForceRotation = false;
	FRotator ForceTargetRotation = FRotator::ZeroRotator;

private:
	bool bForcedSpeed = false;
	float PreForcedSpeed = 0.f;
};
