#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "UObject/Object.h"
#include "BTTask_SetActivityIgnoredActor.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_SetActivityIgnoredActor : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_SetActivityIgnoredActor();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorToIgnore;
};
