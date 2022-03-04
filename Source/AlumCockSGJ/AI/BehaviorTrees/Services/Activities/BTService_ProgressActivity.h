#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ProgressActivity.generated.h"

// TODO utilize. just a draft
UCLASS()
class ALUMCOCKSGJ_API UBTService_ProgressActivity : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ProgressActivity();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
