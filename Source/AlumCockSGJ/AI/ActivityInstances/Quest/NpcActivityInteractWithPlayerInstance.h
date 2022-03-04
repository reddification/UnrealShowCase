#pragma once

#include "CoreMinimal.h"
#include "NpcQuestActivityInstance.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Interfaces/StatefulActivity.h"
#include "NpcActivityInteractWithPlayerInstance.generated.h"

class UNpcQuestActivityInteractWithPlayerSettings;

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityInteractWithPlayerInstance : public UNpcQuestActivityInstance, public IStatefulActivity
{
	GENERATED_BODY()

public:
	virtual void Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController) override;
	virtual FGameplayTag GetStateTag() const override;

private:
	UPROPERTY()
	const UNpcQuestActivityInteractWithPlayerSettings* QuestSettings;
};

UCLASS()
class UNpcQuestActivityInteractWithPlayerSettings : public UNpcQuestActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag InitialNpcState;
};
