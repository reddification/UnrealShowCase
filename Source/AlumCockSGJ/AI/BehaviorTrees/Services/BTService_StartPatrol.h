#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_StartPatrol.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_StartPatrol : public UBTService
{
	GENERATED_BODY()
	
public:
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UBTService_StartPatrol();
private:
	void SetIsPatrolling(UBehaviorTreeComponent& OwnerComp, bool bState);
};
