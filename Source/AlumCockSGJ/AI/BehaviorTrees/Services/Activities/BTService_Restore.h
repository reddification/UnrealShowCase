#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UObject/Object.h"
#include "BTService_Restore.generated.h"

UCLASS(Category="Activities")
class ALUMCOCKSGJ_API UBTService_Restore : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Restore();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
