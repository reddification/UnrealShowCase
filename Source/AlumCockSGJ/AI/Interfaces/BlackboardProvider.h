#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "UObject/Interface.h"
#include "BlackboardProvider.generated.h"

UINTERFACE()
class UBlackboardProvider : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IBlackboardProvider
{
	GENERATED_BODY()

public:
	virtual UBlackboardComponent* GetBlackboard() const { return nullptr; } 
};
