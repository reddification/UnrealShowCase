#pragma once

#include "CoreMinimal.h"
#include "NavLinkCustomComponent.h"
#include "AI/Navigation/NavLinkProxies/BaseSmartNavLinkProxy.h"
#include "BaseSmartLinkComponent.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBaseSmartLinkComponent : public UNavLinkCustomComponent
{
	GENERATED_BODY()

public:
	void SetSmartNavLinkProxyOwner(UObject* Owner);
	
protected:
	virtual void OnLinkMoveFinished(UObject* PathComp) override;

private:
	class IBaseSmartNavLinkProxy* OwnerSmartLinkProxy;
};
