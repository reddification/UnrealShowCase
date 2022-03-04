#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterAnimInstance.h"
#include "Data/Side.h"
#include "HumanoidCharacterAnimInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UHumanoidCharacterAnimInstance : public UBaseCharacterAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeBeginPlay() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void ApplyState(const class UBaseNpcStateSettings* NpcState) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bProning;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bOutOfStamina;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSwimming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bClimbingLadder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bZiplining;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESide WallrunSide = ESide::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSliding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCarrying = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBackingOff = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFleeing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FVector RightFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FVector LeftFootEffectorLocation = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FVector LeftLegJointTargetOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FVector RightLegJointTargetOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FVector PelvisOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FRotator LeftFootRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FRotator RightFootRotator = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient, Category = "Character|IK")
	FRotator TurnHeadTowards = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator RotationSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ClimbingRatio = 0.0f;
	
	TWeakObjectPtr<class ABaseHumanoidCharacter> HumanoidCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f))
	float HeadRotationInterpolationSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f))
	float Stamina = 1.f;
	
private:
	FRotator LastCharacterRotation = FRotator::ZeroRotator;
};
