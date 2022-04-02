#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

class FBaseSavedMove : public FSavedMove_Character
{

public:
	virtual void Clear() override;
	virtual uint8 GetCompressedFlags() const override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMovePtr, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* Character) override;
};

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
	bool CanApplyCustomRotation() const;

	bool bForceRotation = false;
	FRotator ForceTargetRotation = FRotator::ZeroRotator;

private:
	bool bForcedSpeed = false;
	float PreForcedSpeed = 0.f;
};
