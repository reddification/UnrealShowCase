#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "AI/Data/NpcActivityBaseSettings.h"
#include "AI/Interfaces/ObservationActivity.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "NpcActivityInteractWithItemAtLocationInstance.generated.h"

class UNpcActivityInteractWithItemSettings;
class ANpcBaseInteractiveActor;
UCLASS()
class ALUMCOCKSGJ_API UNpcActivityInteractWithItemAtLocationInstance : public UNpcActivityInstanceBase,
	public ITraversalActivity,
	public IObservationActivity
{
	GENERATED_BODY()

public:
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) override;
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData) override;
	virtual bool StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
		FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetActorOnFail) override;
	// virtual bool StopInteracting(const AAIController* AIController,
	// 	FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback) override;
	virtual float Suspend(AAIController* AIController, bool bAbortInteraction) override;
	virtual void Observe(const AAIController* AIController, UBlackboardComponent* Blackboard) override;
	virtual void ProgressIteration(int IterationChange) override;
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus) override;
	
private:
	int CurrentInteractionIndex = -1;
	FTimerHandle InteractionTimer;
	void OnInteractionCompleted();
	
	UPROPERTY()
	mutable UNpcActivityInteractWithItemSettings const * CurrentActivitySettings = nullptr;

	const UNpcActivityInteractWithItemSettings* GetCurrentActivitySettings() const;
};

USTRUCT(BlueprintType)
struct FNpcInteractionOption
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<ANpcBaseInteractiveActor> InteractiveActorClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	FDataTableRowHandle WorldLocationDTRH;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Duration = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InteractionTag;
};

UCLASS()
class UNpcActivityInteractWithItemSettings : public UNpcActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FNpcInteractionOption> InteractionsSchedule;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRandom = false;
};
