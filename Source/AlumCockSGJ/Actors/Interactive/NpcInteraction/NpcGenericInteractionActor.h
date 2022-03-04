#pragma once

#include "CoreMinimal.h"
#include "NpcBaseInteractiveActor.h"
#include "NpcGenericInteractionActor.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ANpcGenericInteractionActor : public ANpcBaseInteractiveActor
{
	GENERATED_BODY()

public:
	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag()) override;
	virtual bool StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted) override;

protected:
	virtual void BeginPlay() override;

	virtual int GetAvailableInteractionPosition(const FGameplayTag& InteractionTag) const override;
	
private:
	TArray<FTimerHandle> InteractionsTimers;
	TArray<FTimerDelegate> EndTimerDelegates;
	void OnInteractionFinished(int Position);	
};
