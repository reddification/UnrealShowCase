#include "BaseSmartLinkComponent.h"

void UBaseSmartLinkComponent::SetSmartNavLinkProxyOwner(UObject* Owner)
{
	OwnerSmartLinkProxy = Cast<IBaseSmartNavLinkProxy>(Owner);
}

void UBaseSmartLinkComponent::OnLinkMoveFinished(UObject* PathComp)
{
	Super::OnLinkMoveFinished(PathComp);
	if (OwnerSmartLinkProxy)
		OwnerSmartLinkProxy->OnSmartLinkPathCompleted();
}
