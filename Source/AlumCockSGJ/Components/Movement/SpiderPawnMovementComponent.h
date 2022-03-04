// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BasePawnMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SpiderPawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API USpiderPawnMovementComponent : public UBasePawnMovementComponent
{
	GENERATED_BODY()
	
public:
    virtual void JumpStart();
    
    virtual bool IsFalling() const override;

	FORCEINLINE void SetIsFalling(bool bFalling)
	{
		this->bIsFalling = bFalling; 
	}
protected:
	virtual void Move(float DeltaTime) override;
};
