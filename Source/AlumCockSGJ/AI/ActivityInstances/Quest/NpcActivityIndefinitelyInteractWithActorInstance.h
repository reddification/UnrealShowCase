#pragma once

#include "CoreMinimal.h"
#include "NpcQuestActivityInstance.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "NpcActivityIndefinitelyInteractWithActorInstance.generated.h"

class UNpcActivityIndefinitelyInteractWithActorSettings;
UCLASS()
class ALUMCOCKSGJ_API UNpcActivityIndefinitelyInteractWithActorInstance : public UNpcQuestActivityInstance
{
	GENERATED_BODY()

public:
	virtual FInteractionEqsSettings PrepareEqsInteractionSettings() override;

protected:
	virtual FGameplayTag GetInteractionTag() override;
	
private:
	
	UPROPERTY()
	const mutable UNpcActivityIndefinitelyInteractWithActorSettings* IndefiniteInteractionSettings;
	
	const UNpcActivityIndefinitelyInteractWithActorSettings* GetSettings() const;
};

UCLASS()
class UNpcActivityIndefinitelyInteractWithActorSettings : public UNpcQuestActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MustImplement=NpcInteractableActor))
	TArray<TSubclassOf<AActor>> ActorClassToInteractWith;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery WithTags;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InteractionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag NpcState;
	
	// Just an idea
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bTeleportCharacterToInteraction = false;
};
