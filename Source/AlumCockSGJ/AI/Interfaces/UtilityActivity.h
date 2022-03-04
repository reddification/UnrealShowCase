#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AI/Data/NpcActivityTypes.h"
#include "UtilityActivity.generated.h"

UINTERFACE()
class UUtilityActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IUtilityActivity
{
	GENERATED_BODY()

public:
	virtual void ReportActivityEvent(EActivityEventType UtilityChange){}
};
