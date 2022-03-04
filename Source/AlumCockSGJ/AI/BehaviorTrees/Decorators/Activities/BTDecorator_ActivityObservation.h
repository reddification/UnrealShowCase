#pragma once

#include "CoreMinimal.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_ActivityObservation.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTDecorator_ActivityObservation : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_ActivityObservation();
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual void OnNodeDeactivation(FBehaviorTreeSearchData& SearchData, EBTNodeResult::Type NodeResult) override;
	
private:
	UNpcActivityInstanceBase* GetActivityInstance(UBehaviorTreeComponent& OwnerComp);
};
