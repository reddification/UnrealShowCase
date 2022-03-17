#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "AI/Interfaces/AssessableActivity.h"
#include "AI/Interfaces/ObservationActivity.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "NpcActivityGatherItemsAtLocationInstance.generated.h"

USTRUCT()
struct FRememberedActorData
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Actor;
	
	FVector RememberedActorLocation;
	float SquaredDistanceToBase;
	float RememberedActorAtWorldTime;
};

class UNpcActivityGatherItemsAtLocationSettings;

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityGatherItemsAtLocationInstance : public UNpcActivityInstanceBase,
	public IObservationActivity,
	public ITraversalActivity
{
	GENERATED_BODY()

public:
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) override;
	// TODO consider adding EQS to activity settings instead of "hard tree'ing" it
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& NextLocationData) override;
	virtual void Observe(const AAIController* AIController, UBlackboardComponent* Blackboard) override;
	virtual bool StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
		FNpcActivityLatentActionStateChangedEvent* LatentActionCallback, bool bResetActorOnFail) override;
	virtual float StopInteracting(const AAIController* AIController,
	                              FNpcActivityLatentActionStateChangedEvent* LatentActionCallback,
	                              bool bResetActorOnFail) override;
	virtual void ProgressIteration(int IterationChange) override;
	
protected:
	bool IsRememberHowManyItemsGathered(float WorldTime) const;
	virtual void RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard) override;
	virtual void OnInteractionActorAvailableStateChanged(AActor* InteractiveActor, bool bAvailable) override;
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus) override;
	
private:
	float RememberHowManyCollectedExpires = -FLT_MAX;
	
	void ResetNumberOfItemsMemory();
	void ResetLastActorMemory();
	
	FRememberedActorData RememberedActorData;
	
	int ActivityItemsCountLeft = 0;
	int LastLocationIndex = 0;
	int ActivityCompletedTimes = 0;
	int UseEqsRoamCount = 0;
	const UNpcActivityGatherItemsAtLocationSettings* GetSettings() const;
	const mutable UNpcActivityGatherItemsAtLocationSettings* GatherItemsSettings;

	FTimerHandle RememberNumberOfItemsResetTimer;
	FTimerHandle RememberLastItemPositionResetTimer;
};

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityGatherItemsAtLocationSettings : public UNpcActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	TArray<FDataTableRowHandle> LocationsToSearchItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETraversalType LocationsTraversalType;

	// If traversed all locations with no result, roam randomly with EQS this number of times
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0, EditCondition="LocationsTraversalType==ETraversalType::Sequential"))
	int RoamWithEqsOnTraversalEndCount = 3;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int RequiredCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float NumberOfItemsMemoryLifespanSeconds = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float LastObservedActorMemoryLifespanSeconds = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxItemRecognitionRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag InteractionTag;
};