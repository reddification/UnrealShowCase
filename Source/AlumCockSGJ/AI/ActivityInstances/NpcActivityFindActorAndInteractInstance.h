#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "NpcActivityFindActorAndInteractInstance.generated.h"

class UNpcActivityFindActorAndInteractSettings;
UCLASS()
class ALUMCOCKSGJ_API UNpcActivityFindActorAndInteractInstance : public UNpcActivityInstanceBase
{
	GENERATED_BODY()

public:
	virtual FInteractionEqsSettings PrepareEqsInteractionSettings() override;
	virtual void SetActorToIgnore(AActor* Actor) override;
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) override;
	virtual bool StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
	                              FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetActorOnFail) override;
	void OnInteractionCompleted();
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus) override;
	virtual bool StopInteracting(const AAIController* AIController, FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback,
		bool bResetInteractionActorOnFail) override;
	
protected:
	virtual void OnInteractionActorAvailableStateChanged(AActor* InteractiveActor, bool bAvailable) override;
	virtual void RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard) override;
	
private:
	FTimerHandle InteractionTimer;
	int CurrentInteractionOptionIndex = 0;

	UPROPERTY()
	mutable UNpcActivityFindActorAndInteractSettings const * RestSettings = nullptr;

	const UNpcActivityFindActorAndInteractSettings* GetRestSettings() const;
};

UCLASS()
class UNpcActivityFindActorAndInteractSettings : public UNpcActivityBaseSettings 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FRestActivityInteractionActorSettings> InteractionOptions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float IgnoreActorWhenUnavailableTime = 30.f;
};