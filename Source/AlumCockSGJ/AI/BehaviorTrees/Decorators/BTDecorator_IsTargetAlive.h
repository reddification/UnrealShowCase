#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsTargetAlive.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTDecorator_IsTargetAlive : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_IsTargetAlive();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorKey;

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

};
