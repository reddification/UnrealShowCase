#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NpcController.generated.h"

UINTERFACE()
class UNpcController : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API INpcController
{
	GENERATED_BODY()

public:
	virtual bool IsInDanger() { return false; }
	virtual void SetAIEnabled(bool bEnabled) {}
};
