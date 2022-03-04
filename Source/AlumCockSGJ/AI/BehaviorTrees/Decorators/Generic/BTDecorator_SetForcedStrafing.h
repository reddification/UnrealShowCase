#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "UObject/Object.h"
#include "BTDecorator_SetForcedStrafing.generated.h"

struct FBTMemory_ForcedStrafing
{
	bool bInitialForceStrafing = false;
};

UCLASS(Category="Generic")
class ALUMCOCKSGJ_API UBTDecorator_SetForcedStrafing : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_SetForcedStrafing();
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bForceStrafing = false;
};
