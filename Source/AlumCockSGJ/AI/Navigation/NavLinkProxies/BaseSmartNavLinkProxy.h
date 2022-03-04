#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BaseSmartNavLinkProxy.generated.h"

UINTERFACE()
class UBaseSmartNavLinkProxy : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IBaseSmartNavLinkProxy
{
	GENERATED_BODY()

public:
	virtual void OnSmartLinkPathCompleted() {}
};
