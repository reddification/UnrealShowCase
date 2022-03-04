#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_LookForItemsOfActivityInterest.generated.h"

class UNpcActivityInstanceBase;
UCLASS(Category="Activities")
class ALUMCOCKSGJ_API UBTService_LookForItemsOfActivityInterest : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_LookForItemsOfActivityInterest();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
