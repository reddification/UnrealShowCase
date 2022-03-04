#pragma once

#include "AI/Interfaces/NpcInteractableActor.h"
#include "NpcActorInteractionData.generated.h"

UENUM(BlueprintType)
enum class ENpcActorInteractionState : uint8
{
	None,
	Approaching,
	Interacting,
	Releasing
};

USTRUCT()
struct FNpcActorInteractionData
{
	GENERATED_BODY()
	
	UPROPERTY()
	AActor* ActorToInteract;

	INpcInteractableActor* InteractableActor = nullptr;

	float RememberUntilWorldTime = 0.f;
	uint32 ActivityId = -1;
	
	INpcInteractableActor* GetInteractableActor() const
	{
		return ActorToInteract ? InteractableActor : nullptr;
	}
	
	ENpcActorInteractionState State = ENpcActorInteractionState::None;

	void SetApproaching(AActor* NewActor)
	{
		SetState(NewActor, ENpcActorInteractionState::Approaching);
	}

	void SetInteracting(AActor* NewActor)
	{
		SetState(NewActor, ENpcActorInteractionState::Interacting);
	}

	void SetReleasing()
	{
		State = ENpcActorInteractionState::Releasing;
	};
	
	void SetState(AActor* NewActor, ENpcActorInteractionState NewState, float RememberForDuration = 60.f)
	{
		ActorToInteract = NewActor;
		InteractableActor = Cast<INpcInteractableActor>(NewActor);
		State = NewState;
		if (NewActor)
			RememberUntilWorldTime = NewActor->GetWorld()->GetTimeSeconds() + RememberForDuration;
	}
	
	void Reset()
	{
		State = ENpcActorInteractionState::None;
		ActorToInteract = nullptr;
		InteractableActor = nullptr;
		RememberUntilWorldTime = 0.f;
		ActivityId = -1;
	}

	bool IsNone() const
	{
		return !IsValid(ActorToInteract) || State == ENpcActorInteractionState::None;
	}
	
	bool IsInteracting() const
	{
		return IsValid(ActorToInteract) && State == ENpcActorInteractionState::Interacting;
	}

	bool IsApproaching() const
	{
		return IsValid(ActorToInteract) && State == ENpcActorInteractionState::Approaching;
	}

	bool IsReleasing() const
	{
		return State == ENpcActorInteractionState::Releasing; 
	}
};