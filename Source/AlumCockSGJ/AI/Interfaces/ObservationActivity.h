#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AIController.h"
#include "ObservationActivity.generated.h"

UINTERFACE()
class UObservationActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IObservationActivity
{
	GENERATED_BODY()

public:
	virtual void Observe(const AAIController* AIController, UBlackboardComponent* Blackboard) {}

};
