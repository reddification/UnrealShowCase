#pragma once

#include "CoreMinimal.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "Data/Quests/QuestAction.h"
#include "NpcQuestActivityInstance.generated.h"

class UNpcQuestActivityBaseSettings;

UCLASS()
class ALUMCOCKSGJ_API UNpcQuestActivityInstance : public UNpcActivityInstanceBase
{
	GENERATED_BODY()

public:
	virtual void Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController) override;
	virtual void Resume() override;
	virtual void Suspend(AAIController* AIController, bool bAbortInteraction) override;
	void FinishQuestActivity(EActivityEventType Result = EActivityEventType::Completed);
	void SetStopConditions(const AAIController* AIController, const FNpcQuestBehaviorDescriptor& QuestBehaviorDescriptor);
	virtual void ReportActivityEvent(EActivityEventType UtilityChange) override;
	
private:
	FDelegateHandle CharacterStateChangedEventHandle;
	FDelegateHandle WorldStateChangedEventHandle;

	FGameplayTagQuery UntilWorldState;
	FGameplayTagQuery UntilCharacterState;
	
	void OnWorldStateChanged(const FGameplayTagContainer& NewWorldState);
	void OnCharacterStateChanged(const FGameplayTagContainer& NewCharacterState);
	void ActivityTimerElapsed();

	bool bStateAchieved = false;

	FTimerHandle Timer;
	int RetriesLeft = -1;

	UNpcQuestActivityBaseSettings* QuestSettings = nullptr;
};

UCLASS()
class UNpcQuestActivityBaseSettings : public UNpcActivityBaseSettings
{
	GENERATED_BODY()
public:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float TimeLimit = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bQuestActivitySuccessOnTimerEnd = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bQuestActivitySuccessOnRetriesEnd = false;
	
	// -1 == Infinite
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(UIMin = -1, ClampMin = -1))
	int RetryLimit = -1;
};
