#pragma once

#include "CoreMinimal.h"
#include "AI/Data/DelegatesAI.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "NpcInteractableActor.generated.h"

USTRUCT(BlueprintType)
struct FNpcInteractionStopResult
{
	GENERATED_BODY()

	FNpcInteractionStopResult() {  }
	FNpcInteractionStopResult(float delay) : bStopped(true), Delay(delay) {  }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bStopped = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Delay = -1.f;
};

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

	// TODO get rid of BP native events
	
	UFUNCTION(BlueprintNativeEvent)
	bool TryStartNpcInteraction(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag);
	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Npc,
		const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) { return false; }
	
	UFUNCTION(BlueprintNativeEvent)
	FNpcInteractionStopResult StopNpcInteraction(ABaseCharacter* Npc, bool bInterupted = false);
	virtual FNpcInteractionStopResult StopNpcInteraction_Implementation(
		ABaseCharacter* Npc, bool bInterupted = false) { return FNpcInteractionStopResult(); }

	UFUNCTION(BlueprintNativeEvent)
	bool IsNpcInteractionAvailable(const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) const;
	virtual bool IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag = FGameplayTag::EmptyTag) const { return false; }

	virtual bool GetInteractionLocation(const FGameplayTag& InteractionTag, FVector& OutLocation) { return false; }
	
	virtual bool GetNpcActiveInteractionTag(ABaseCharacter* Npc, FGameplayTag& OutTag) const { return false; }
};
