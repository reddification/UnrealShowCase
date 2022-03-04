#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ThrowGrenade.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_ThrowGrenade : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ThrowGrenade();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MinAttackRange = 1500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAttackRange = 2500.f;
};
