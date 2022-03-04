#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/Interface.h"
#include "AssessableActivity.generated.h"

UINTERFACE()
class UAssessableActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IAssessableActivity
{
	GENERATED_BODY()

public:
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) { return true; }
	virtual void OnActivityCompleted(AAIController* AIController, UBlackboardComponent* Blackboard) { }
};
