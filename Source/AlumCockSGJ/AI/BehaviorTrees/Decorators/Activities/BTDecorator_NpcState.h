#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NpcState.generated.h"

struct FNpcStateMemory
{
	FGameplayTag InitialTag;
};

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTDecorator_NpcState : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_NpcState();
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FNpcStateMemory); }
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bFromActivity = false; 
	
	//TODO would be cooler to have FGameplayTagQuery or FGameplayTagContainer so that states are stackable
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFromActivity"))
	FGameplayTag StateTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bFromActivity"))
	FBlackboardKeySelector NpcStateParameterKey;

private:
	FGameplayTag GetActualTag(UBehaviorTreeComponent& OwnerComp) const;
};
