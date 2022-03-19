#pragma once

#include "CoreMinimal.h"
#include "NpcBaseInteractiveActor.h"
#include "NpcPiano.generated.h"

// TODO use NpcGenericInteractionActor with BPImplementableEvents for music instead
UCLASS()
class ALUMCOCKSGJ_API ANpcPiano : public ANpcBaseInteractiveActor
{
	GENERATED_BODY()

public:
	ANpcPiano();
	
	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag()) override;
	virtual FNpcInteractionStopResult StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAudioComponent* AudioComponent;

private:
	FTimerHandle SitTimer;
	FTimerHandle StandUpTimer;

	void OnSat();
	void OnStoodUp();
};
