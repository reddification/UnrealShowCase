#pragma once

#include "CoreMinimal.h"
#include "AIBaseActivityComponent.h"
#include "AI/Data/NpcBaseActivityWrapper.h"
#include "AI/Data/NpcBehaviorSettings.h"
#include "AI/Interfaces/BlackboardProvider.h"
#include "Data/Quests/QuestAction.h"
#include "Subsystems/WorldStateSubsystem.h"
#include "AINpcActivityComponent.generated.h"

DECLARE_DELEGATE(FQuestActivityFinishedEvent)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UAINpcActivityComponent : public UAIBaseActivityComponent, public IBlackboardProvider
{
	GENERATED_BODY()

	struct FPlannedActivity
	{
		FPlannedActivity() {  }
		
		FPlannedActivity(UNpcActivityInstanceBase* activityInstance, UBehaviorTree* behaviorTree, float activityNecessity,
			ENpcActivityType npcActivityType)
		{
			bStarted = false;
			bInteractionInterrupted = false;
			ActivityInstance = activityInstance;
			BehaviorTree = behaviorTree;
			ActivityNecessity = activityNecessity;
			NpcActivityType = npcActivityType;
		}
		
		bool bStarted = false;
		bool bInteractionInterrupted = false;
		float ActivityNecessity = 0.f;
		ENpcActivityType NpcActivityType = ENpcActivityType::None;

		UPROPERTY()
		UBehaviorTree* BehaviorTree = nullptr;
		
		UPROPERTY()
		UNpcActivityInstanceBase* ActivityInstance = nullptr;
	};
	
public:
	UAINpcActivityComponent();
	UNpcActivityInstanceBase* GetActivityInstance();

	virtual UBlackboardComponent* GetBlackboard() const override;
	void Rest(float DeltaSeconds);
	void RunQuestActivity(const FNpcQuestBehaviorDescriptor& QuestBehaviorDescriptor);

	virtual void InitializeNpc();
	
	const FNpcActorInteractionData* GetCurrentNpcInteractionData() const;
	void SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State, uint32 ActivityId);
	void SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State);
	void ResetActorInteractionData();
	
	void StartDialogueMode(bool bFollowPlayer);
	void EndDialogueMode();
	virtual bool IsInteractionInterruptionRequired() const;

	ENpcActivityType GetRunningActivityType() const { return RunningActivityType; }
	void ProlongInteractionMemory(float ProlongationTime);

	FQuestActivityFinishedEvent QuestActivityFinishedEvent;
	
protected:
	virtual void BeginPlay() override;
	virtual void InitializeActivity() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag RunningActivityTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag QuestSubtreeTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer InteruptableByDialogue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UtilityUpdateInterval = 5.f;

private:
	UPROPERTY()
	TArray<FNpcRestActivityWrapper> NpcRestActivities;
	
	UPROPERTY()
	TArray<FNpcWorkActivityWrapper> NpcWorkActivities;

	FNpcQuestActivityWrapper RunningQuestActivity;

	UPROPERTY()
	UNpcActivityInstanceBase* RunningActivity = nullptr;
	
	int32 RunningWorkActivityIndex = -1;
	int32 RunningRestActivityIndex = -1;

	float ActivityEnergy = 1.f;
	float Boredom = 0.f;
	ENpcActivityType RunningActivityType = ENpcActivityType::None;
	bool bInDialogue = false;
	bool bInConversation = false;
	
	FTimerHandle UtilityTimer;
	FTimerHandle DelayedActivityLaunchTimer;

	void RunRandomRestActivity(UBehaviorTreeComponent* BTComponent);
	void PlanActivity();
	void RunMaxUtilityActivity(UBehaviorTreeComponent* BTComponent);
	void ResetRunningQuestActivity();
	void LaunchPlannedActivity();

	void SelectNextActivity();
	void SetRunningActivityType(ENpcActivityType NewType);
	
	void UpdateUtility();
	void SetupActivityWrapperBase(const FNpcBehaviorActivityDescriptor& BehaviorDescriptor,
		UNpcActivityInstanceBase* ActivityInstance, FNpcBaseActivityWrapper& NewDescriptor) const;
	void OnEnergyChanged(float EnergyChange);
	void OnRegularNpcActivityStateChanged(const UNpcActivityInstanceBase* ActivityInstance, bool bActive);
	void SetupRestActivityBehavior(const FNpcActivityRestBehavior& BehaviorDescriptor);
	void SetupWorkActivityBehavior(const FNpcActivityWorkBehavior& BehaviorDescriptor);
	void SetupReactionBehavior(UBehaviorTreeComponent* BTComponent,
	                           const FNpcBehaviorReactionDescriptor& BehaviorDescriptor);
	void OnWorkActivityEventOccured(EActivityEventType EventType);
	void OnRestActivityEventOccured(EActivityEventType EventType);
	void OnQuestActivityEventOccured(EActivityEventType EventType);
	
	UBehaviorTreeComponent* GetBehaviorTreeComponent() const;

	FNpcActorInteractionData CurrentInteractionData;
	FPlannedActivity PlannedActivity;
};
