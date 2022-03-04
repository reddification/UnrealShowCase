#pragma once

#include "CoreMinimal.h"
#include "AIBaseController.h"
#include "AICitizenController.h"
#include "AI/Interfaces/ActivityCharacterProvider.h"
#include "AI/Interfaces/BlackboardProvider.h"
#include "AI/Interfaces/FeelingsPropagator.h"
#include "AI/Interfaces/FeelingsProvider.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcController.h"
#include "AI/Interfaces/NpcInteractionManager.h"
#include "Interfaces/DialogueParticipantManager.h"
#include "AIDogController2.generated.h"

// TODO блять столько копипасты Александр ты ебанулся?
UCLASS()
class ALUMCOCKSGJ_API AAIDogController2 : public AAIBaseController, public INpcController,
	public IBlackboardProvider, public IActivityCharacterProvider,
	public INpcActivityManager, public INpcInteractionManager,
	public IDialogueParticipantManager
{
	GENERATED_BODY()

public:
	AAIDogController2(const FObjectInitializer& ObjectInitializer);

	virtual UNpcActivityInstanceBase* GetRunningActivity() override;
	virtual UBlackboardComponent* GetBlackboard() const override;
	virtual ABaseCharacter* GetActivityCharacter() const override;
	virtual void Restore(float DeltaSeconds) override;
	virtual void RunQuestActivity(const FNpcQuestBehaviorDescriptor& BehaviorDescriptor) override;
	virtual ENpcActivityType GetRunningActivityType() const override;

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
	
	TSubclassOf<UNavigationQueryFilter> InitialNavigationFilterClass;

	void InitializeNpc(INpcCharacter* NpcCharacter);
};
