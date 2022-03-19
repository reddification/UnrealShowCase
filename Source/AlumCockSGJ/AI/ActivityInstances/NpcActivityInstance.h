#pragma once

#include "CoreMinimal.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/BehaviorTrees/Tasks/Activities/BTTask_RunActivityEQSQuery.h"
#include "AI/Data/NpcActorInteractionData.h"
#include "AI/Data/DelegatesAI.h"
#include "AI/Data/InteractionEqsSettings.h"
#include "AI/Data/NpcActivityTypes.h"
#include "AI/Data/NpcBehaviorSettings.h"
#include "AI/Interfaces/AssessableActivity.h"
#include "AI/Interfaces/InteractionActivity.h"
#include "AI/Interfaces/UtilityActivity.h"
#include "Perception/AIPerceptionTypes.h"
#include "NpcActivityInstance.generated.h"

class UNpcActivityBaseSettings;
class AAICitizenController;
class UMLGameInstance;
class UBlackboardComponent;

USTRUCT()
struct FActorInteractionMemory
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* ActorToInteract;

	float UntilWorldTime = 0.f;
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FActivityProgressEvent, float EnergyChange, float BoredomChange, float UtilityChange)

/* Fundamental flaws:
 * 1. Activity knows about UBlackboardComponent. Ideally activities should be independent from BTs and BBs. They just contain logic
 * 2. No clear way to handle latent BT tasks, have to inject callbacks from BT inside activities.
 *		I assume this can lead to undefined behavior if subtree got aborted by a higher priority subtree 
 *
 * TODO fix when I am smarter
*/

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityInstanceBase : public UObject, public IAssessableActivity, public IUtilityActivity,
	public IInteractionActivity
{
	GENERATED_BODY()
	
public:
	// TODO in the end of the day ActivityStates and Instances should be independent from BlackboardComponent. Otherwise the system is tightly coupled
	void RestoreBlackboardState(UBlackboardComponent* Blackboard);
	virtual void Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController);

	const UNpcActivityBaseSettings* GetActivitySettings() const { return ActivitySettings; }

	virtual void Resume();
	virtual float Suspend(AAIController* AIController, bool bAbortInteraction);

	bool IsActive() const { return bActive; }
	virtual void SetActive(bool bActiveNew);
	
	virtual void Complete(UBlackboardComponent* BlackboardComponent);
	virtual void ReportActivityEvent(EActivityEventType ActivityEventType) override;
	
	void StartObserving(AAIController* AIController);	
	void StopObserving(AAIController* AIController);

	virtual bool StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
	    FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetInteractionActorOnFail) override;
	virtual float StopInteracting(const AAIController* AIController,
	                              FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback,
	                              bool bResetInteractionActorOnFail) override;

	virtual void ProgressIteration(int IterationChange) {}
	virtual void SetActorToIgnore(AActor* Actor);
	virtual FInteractionEqsSettings PrepareEqsInteractionSettings() { return FInteractionEqsSettings(); }
	virtual void Progress(float DeltaSeconds);
	virtual UEnvQuery* GetQueryTemplate(const FGameplayTag& QueryTag);

	FNpcActivityEvent NpcActivityEvent;
	FActivityProgressEvent ActivityProgressEvent;
	FNpcActivityStateChangedEvent NpcActivityStateChangedEvent;
	
	const AQuestLocation* GetCoreLocation() const;
	
protected:
	virtual void RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard);
	void ResetInteractionActor();

	UPROPERTY()
	const UNpcActivityBaseSettings* ActivitySettings;

	UPROPERTY()
	const mutable AQuestLocation* CoreLocation;
	
	bool bActive = false;

	FNpcActivityLatentActionStateChangedEvent* ActiveLatentActionCallback;
	FDelegateHandle InteractionActorAvailabilityDelegateHandler;
	FDelegateHandle InteractionDelegateHandle;

	virtual void OnInteractionActorAvailableStateChanged(AActor* InteractiveActor, bool bAvailable);
	virtual void SetInteractionActor(UBlackboardComponent* Blackboard, AActor* ObservedActor,
		ENpcActorInteractionState InteractionState = ENpcActorInteractionState::Approaching);
	void ClearActorInteractionData() const;
	void UnsubscribeFromInteractionAvailability();
	virtual void PrepareForInteraction();
	// TODO character shouldn't really be here...
	virtual void OnInteractionStateChanged(ANpcBaseInteractiveActor* InteractiveActor, ABaseCharacter* Interactor,
		ENpcActivityLatentActionState NpcActivityLatentActionState);
	float StopInteracting(const FNpcActorInteractionData* ActorInteractionData);
	const FNpcActorInteractionData* GetActorInteractionData() const;
	virtual void SetActorInteractionData(AActor* Actor, ENpcActorInteractionState State);
	virtual void SetActorInteractionDataState(ENpcActorInteractionState State);
	virtual void ProlongInteractionMemory() const;
	
	FVector LastActorInteractionLocation = FVector::ZeroVector;
	
	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus FaiStimulus) {}

	UPROPERTY()
	mutable TArray<FActivityIgnoreActorData> IgnoredActors;

	void ClearNotIgnoredAnymoreActors() const;
	const AQuestLocation* GetClosestQuestLocation(const FDataTableRowHandle& LocationDTRH, const FVector& QuerierLocation) const;

	virtual FGameplayTag GetInteractionTag() { return FGameplayTag::EmptyTag; }

	class INpcInteractionManager* NpcInteractionManager;
	FActorInteractionMemory InteractionMemory;
	
	uint32 GetActivityId() const { return GetUniqueID(); }
};
