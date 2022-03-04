#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SetTagParameter.generated.h"

struct FSetTagParameterMemory
{
	FName PreviousTag;
};

UCLASS(Category="Generic")
class ALUMCOCKSGJ_API UBTDecorator_SetTagParameter : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_SetTagParameter();
	virtual FString GetStaticDescription() const override;
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	virtual uint16 GetInstanceMemorySize() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OutParameterKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag;
};
