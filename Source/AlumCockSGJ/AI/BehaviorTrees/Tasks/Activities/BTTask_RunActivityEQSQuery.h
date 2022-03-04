#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"
#include "BTTask_RunActivityEQSQuery.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_RunActivityEQSQuery : public UBTTask_RunEQSQuery
{
	GENERATED_BODY()

public:
	UBTTask_RunActivityEQSQuery(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag ActivityQueryTag;
};
