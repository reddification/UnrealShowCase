#pragma once

#include "CoreMinimal.h"
#include "AI/Data/DelegatesAI.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "NpcInteractableActor.generated.h"

class ABaseCharacter;
UINTERFACE()
class UNpcInteractableActor : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API INpcInteractableActor
{
	GENERATED_BODY()

public:
	mutable FInteractionAvailableStateChangedEvent InteractionAvailableStateChangedEvent;
	mutable FNpcInteractionStateChangedEvent InteractionStateChangedEvent;

	UFUNCTION(BlueprintNativeEvent)
	bool TryStartNpcInteraction(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag);
	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Npc,
		const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) { return false; }
	
	UFUNCTION(BlueprintNativeEvent)
	bool StopNpcInteraction(ABaseCharacter* Npc, bool bInterupted = false);
	virtual bool StopNpcInteraction_Implementation(ABaseCharacter* Npc, bool bInterupted = false) { return false; }

	UFUNCTION(BlueprintNativeEvent)
	bool IsNpcInteractionAvailable(const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) const;
	virtual bool IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) const { return false; }

	virtual bool GetInteractionLocation(const FGameplayTag& InteractionTag, FVector& OutLocation) { return false; }
	
	virtual bool GetNpcActiveInteractionTag(ABaseCharacter* Npc, FGameplayTag& OutTag) const { return false; }
};
