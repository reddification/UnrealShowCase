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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Climbing")
	bool bClimbingLadder;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Climbing")
	float ClimbingRatio = 0.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline")
	bool bZiplining;	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline")
	FVector ZiplineHandLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wallrun")
	bool bWallRunning;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wallrun")
	ESide WallrunSide = ESide::None;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bSliding;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCarrying = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bBackingOff = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFleeing = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Transient)
	FRotator TurnHeadTowards = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator RotationSpeed;
	
	TWeakObjectPtr<class ABaseHumanoidCharacter> HumanoidCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f))
	float HeadRotationInterpolationSpeed = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f))
	float Stamina = 1.f;

private:
	FRotator LastCharacterRotation = FRotator::ZeroRotator;
};
