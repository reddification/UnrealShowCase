#pragma once
#include "BaseSmartNavLinkProxy.h"
#include "Navigation/NavLinkProxy.h"
#include "NavLinkCustomComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "NavLinkProxySlide.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ANavLinkProxySlide : public ANavLinkProxy, public IBaseSmartNavLinkProxy
{
	GENERATED_BODY()
	
public:
	ANavLinkProxySlide(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

	virtual void OnSmartLinkPathCompleted() override;
	
private:
	TWeakObjectPtr<ABaseHumanoidCharacter> Character;

	UNavLinkCustomComponent::FOnMoveReachedLink OnMoveReachedLink;
	void OnSlideEnded();

	UFUNCTION()
	void InvokeSlide(AActor* Actor, const FVector& DestinationPoint);
};
