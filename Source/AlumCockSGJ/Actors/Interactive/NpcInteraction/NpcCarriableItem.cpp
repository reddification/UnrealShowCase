#include "NpcCarriableItem.h"

#include "Characters/BaseHumanoidCharacter.h"

ANpcCarriableItem::ANpcCarriableItem()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static mesh"));
	SetRootComponent(StaticMeshComponent);
}

bool ANpcCarriableItem::TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag)
{
	bool bCanStartInteracting = Super::TryStartNpcInteraction_Implementation(Character, InteractionTag);
	if (!bCanStartInteracting)
		return false;
	
	auto HumanoidCharacter = Cast<ABaseHumanoidCharacter>(Character);
	if (!HumanoidCharacter)
		return false;

	auto CarryingComponent = HumanoidCharacter->GetCarryingComponent();
	if (!CarryingComponent->CarryingStateChangedEvent.IsBoundToObject(this))
		CarryingCallbackHandle = CarryingComponent->CarryingStateChangedEvent.AddUObject(this, &ANpcCarriableItem::OnCarryingStateChanged);

	bool bStartCarrying = CarryingComponent->StartCarrying(this);
	if (!bStartCarrying)
		return false;

	FRunningInteractionData RunningInteractionData(Character);
	BeginInteraction(0, RunningInteractionData);
	return bStartCarrying;
}

bool ANpcCarriableItem::StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterruped)
{
	auto HumanoidCharacter = Cast<ABaseHumanoidCharacter>(Character);
	if (!IsValid(HumanoidCharacter))
		return false;

	auto CarryingComponent = HumanoidCharacter->GetCarryingComponent();
	if (!CarryingComponent->CarryingStateChangedEvent.IsBoundToObject(this))
		CarryingCallbackHandle = CarryingComponent->CarryingStateChangedEvent.AddUObject(this, &ANpcCarriableItem::OnCarryingStateChanged);
	
	// SubmitInteractionState(Character, false);
	bool bCanStopInteracting = bInterruped
		? HumanoidCharacter->GetCarryingComponent()->InterruptCarrying()
		: HumanoidCharacter->GetCarryingComponent()->StopCarrying();

	return bCanStopInteracting;
}

void ANpcCarriableItem::ClearInteractionState(ABaseCharacter* Character)
{
	Super::ClearInteractionState(Character);
	auto HumanoidCharacter = Cast<ABaseHumanoidCharacter>(Character);
	HumanoidCharacter->GetCarryingComponent()->CarryingStateChangedEvent.Remove(CarryingCallbackHandle);
}

void ANpcCarriableItem::OnCarryingStateChanged(ABaseHumanoidCharacter* Character, ECarryingState CarryingState)
{
	ENpcActivityLatentActionState LatentActionState;
	switch (CarryingState)
	{
		case ECarryingState::Stopped:
			LatentActionState = ENpcActivityLatentActionState::Completed;
			FinishInteraction(Character);
			break;
		case ECarryingState::Started:
			LatentActionState = ENpcActivityLatentActionState::Started;
			break;
		case ECarryingState::Interruped:
			LatentActionState = ENpcActivityLatentActionState::Aborted;
			FinishInteraction(Character);
			break;
		default:
			LatentActionState = ENpcActivityLatentActionState::Failed;
			FinishInteraction(Character);
			break;
	}

	if (InteractionStateChangedEvent.IsBound())
		InteractionStateChangedEvent.Broadcast(this, Character, LatentActionState);
}
