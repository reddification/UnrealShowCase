#pragma once

#include "CoreMinimal.h"
#include "NpcBaseInteractiveActor.h"
#include "Components/Character/CarryingComponent.h"
#include "NpcCarriableItem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ANpcCarriableItem : public ANpcBaseInteractiveActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ANpcCarriableItem();

	virtual bool TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag = FGameplayTag()) override;
	virtual FNpcInteractionStopResult StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted = false) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	virtual void ClearInteractionState(ABaseCharacter* Character) override;

private:
	void OnCarryingStateChanged(ABaseHumanoidCharacter* Character, ECarryingState CarryingState);
	FDelegateHandle CarryingCallbackHandle;
};
