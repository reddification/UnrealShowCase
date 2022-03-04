#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "BTTask_ReportActivityEvent.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_ReportActivityEvent : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_ReportActivityEvent();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EActivityEventType ActivityEvent;
};
