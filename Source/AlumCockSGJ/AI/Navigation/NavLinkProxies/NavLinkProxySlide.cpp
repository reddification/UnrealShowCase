#include "NavLinkProxySlide.h"

#include "NavLinkCustomComponent.h"
#include "AI/Components/BaseSmartLinkComponent.h"
#include "Characters/BaseHumanoidCharacter.h"

ANavLinkProxySlide::ANavLinkProxySlide(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseSmartLinkComponent>(TEXT("SmartLinkComp")))
{
	bSmartLinkIsRelevant = true;
}

void ANavLinkProxySlide::BeginPlay()
{
	Super::BeginPlay();
	OnSmartLinkReached.AddDynamic(this, &ANavLinkProxySlide::InvokeSlide);
	UBaseSmartLinkComponent* BaseSmartLinkComponent = Cast<UBaseSmartLinkComponent>(GetSmartLinkComp());
	if (IsValid(BaseSmartLinkComponent))
	{
		BaseSmartLinkComponent->SetSmartNavLinkProxyOwner(this);
	}
}

void ANavLinkProxySlide::OnSmartLinkPathCompleted()
{
	IBaseSmartNavLinkProxy::OnSmartLinkPathCompleted();
	OnSlideEnded();
}

void ANavLinkProxySlide::InvokeSlide(AActor* Actor, const FVector& DestinationPoint)
{
	Character = Cast<ABaseHumanoidCharacter>(Actor);
	if (Character.IsValid())
	{
		Character->Crouch();
	}
}

void ANavLinkProxySlide::OnSlideEnded()
{
	if (Character.IsValid())
	{
		Character->UnCrouch();
	}
}