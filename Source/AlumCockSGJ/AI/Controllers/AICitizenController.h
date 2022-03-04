#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "AIController.h"
#include "AI/Data/NpcActorInteractionData.h"
#include "AI/Data/NpcDTR.h"
#include "AI/Interfaces/ActivityCharacterProvider.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcController.h"
#include "AI/Interfaces/BlackboardProvider.h"
#include "AI/Interfaces/FeelingsPropagator.h"
#include "AI/Interfaces/FeelingsProvider.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "AI/Interfaces/NpcInteractionManager.h"
#include "Interfaces/DialogueParticipantManager.h"
#include "Perception/AIPerceptionTypes.h"
#include "AICitizenController.generated.h"

struct FRunningQuestActivityParameters
{
	bool bBlockSenses = false;
};

class ABaseCharacter;
UCLASS()
class ALUMCOCKSGJ_API AAICitizenController : public AAIBaseController, public INpcController,
	public IBlackboardProvider, public IActivityCharacterProvider, public IFeelingsProvider,
	public IFeelingsPropagator, public INpcActivityManager, public INpcInteractionManager,
	public IDialogueParticipantManager
{
	GENERATED_BODY()

public:
    AAICitizenController(const FObjectInitializer& ObjectInitializer);
	virtual UNpcActivityInstanceBase* GetRunningActivity() override;
	virtual UBlackboardComponent* GetBlackboard() const override;
	virtual ABaseCharacter* GetActivityCharacter() const override;
	virtual TArray<FVector> GetAfraidLocations() override;
	virtual TArray<FVector> GetInterestingLocations() override;
	virtual FPropagatedFeelingsData GetAnxietyPropagation() override;
	virtual FPropagatedFeelingsData GetInterestPropagation() override;
	virtual void Restore(float DeltaSeconds) override;
	virtual void RunQuestActivity(const FNpcQuestBehaviorDescriptor& BehaviorDescriptor) override;
	virtual ENpcActivityType GetRunningActivityType() const override;
	virtual bool IsInDanger() override;

	virtual const FNpcActorInteractionData* GetCurrentNpcInteractionData() const override;
	virtual void SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State, uint32 ActivityId) override;
	virtual void SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State) override;
	virtual void ResetActorInteractionData() override;
	virtual void ProlongInteractionMemory(float ProlongationTime) override;
	
	virtual void OnDialogueStarted(bool bFollowPlayer) override;
	virtual void OnDialogueEnded() override;
	virtual void TryAbortDialogue() const override;
	
	virtual bool IsInteractionInterruptionRequired() const override;
	virtual void SetAIEnabled(bool bNewState) override;

	virtual void SetNavigationFilterClass(const TSubclassOf<UNavigationQueryFilter>& CustomNavigationFilter) override;
	virtual void ResetNavigationFilterClass() override;

	virtual void SetPawn(APawn* InPawn) override;
	
protected:
	virtual void BeginPlay() override;
	
private:
	class UAINpcActivityComponent* NpcActivitiesComponent;
	class UAICitizenReactionsComponent* CitizenReactionsComponent;
	FRunningQuestActivityParameters RunningQuestActivityParameters;
	
	void OnQuestActivityFinished();
	TSubclassOf<UNavigationQueryFilter> InitialNavigationFilterClass;

	void InitializeNpc(INpcCharacter* NpcCharacter);
};
