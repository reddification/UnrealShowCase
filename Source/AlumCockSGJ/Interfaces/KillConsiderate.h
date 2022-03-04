#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "KillConsiderate.generated.h"

UINTERFACE()
class UKillConsiderate : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IKillConsiderate
{
	GENERATED_BODY()

public:
	virtual void ReportKill(const AActor* KilledActor) {}
};
