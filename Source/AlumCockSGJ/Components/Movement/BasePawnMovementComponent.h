// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "BasePawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API UBasePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	virtual void JumpStart();
	
	UPROPERTY(EditAnywhere)
	float PawnHalfHeight = 0.0f;

	virtual bool IsFalling() const override;

protected:
	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.0f;

	UPROPERTY(EditAnywhere)
	bool bEnableGravity = true;

	UPROPERTY(EditAnywhere)
	float InitialJumpSpeed = 500.0f;

	virtual void Move(float DeltaTime);
	
	float VerticalSpeed = 0; // V = V0 + gt for falling
	bool bIsFalling = false;
};
