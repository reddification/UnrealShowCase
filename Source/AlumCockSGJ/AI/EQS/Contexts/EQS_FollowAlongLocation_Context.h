#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQS_FollowAlongLocation_Context.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UEQS_FollowAlongLocation_Context : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
