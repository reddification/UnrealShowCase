#pragma once

#include "CoreMinimal.h"
#include "Components/Movement/HumanoidCharacterMovementComponent.h"
#include "AICharacterMovementComponent.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAICharacterMovementComponent : public UBaseCharacterMovementComponent
{
	GENERATED_BODY()

public:
	void SetIsPatrolling(bool bNewState) { bPatrolling = bNewState; }
	void SetIsPursuing(bool bNewState) { bPursuing = bNewState; }

	virtual float GetMaxSpeed() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed|AI", meta=(ClampMin=0, UIMin=0))
	float PatrolSpeed = 250.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement|Speed|AI", meta=(ClampMin=0, UIMin=0))
	float PursueSpeed = 1000.f;
	
private:
	bool bPatrolling = false;
	bool bPursuing = false;
};
