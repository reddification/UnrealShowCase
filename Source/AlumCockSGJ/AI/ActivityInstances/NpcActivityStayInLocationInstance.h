#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "NpcActivityStayInLocationInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityStayInLocationInstance : public UNpcActivityInstanceBase, public ITraversalActivity
{
	GENERATED_BODY()

public:
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData) override;
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) override;
	
protected:
	virtual void RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard) override;

private:
	bool bAtLocation = false;
	FVector FinalLocation = FVector::ZeroVector;
};
