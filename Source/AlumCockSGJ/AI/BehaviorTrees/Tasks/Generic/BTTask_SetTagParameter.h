#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UObject/Object.h"
#include "BTTask_SetTagParameter.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_SetTagParameter : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetTagParameter();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OutParameterKey;
};
