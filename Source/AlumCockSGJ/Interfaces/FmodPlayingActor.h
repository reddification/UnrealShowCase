#pragma once

#include "CoreMinimal.h"
#include "FMODEvent.h"
#include "UObject/Interface.h"
#include "FmodPlayingActor.generated.h"

UINTERFACE()
class UFmodPlayingActor : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IFmodPlayingActor
{
	GENERATED_BODY()

public:
	virtual void PlayFmodEvent(UFMODEvent* FmodEvent) {}
};
