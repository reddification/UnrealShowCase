#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_SetFocus.generated.h"

struct FBTMemory_SetFocus
{
	bool bInitialAiForcedStrafing = false;
};

UCLASS(Category="Generic")
class ALUMCOCKSGJ_API UBTDecorator_SetFocus : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_SetFocus();
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTMemory_SetFocus); }
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorToFocusOnKey;
};
