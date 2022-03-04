#pragma once

#include "CoreMinimal.h"
#include "NpcQuestActivityInstance.h"
#include "AI/Interfaces/ReactionActivity.h"
#include "AI/Interfaces/StatefulActivity.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "NpcActivityIndefinetilyStayAtLocation.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityIndefinetilyStayAtLocation : public UNpcQuestActivityInstance,
	public IReactionActivity, public IStatefulActivity, public ITraversalActivity
{
	GENERATED_BODY()

public:
	virtual void Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController) override;
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData) override;
	virtual FGameplayTag GetStateTag() const override;
	virtual FGameplayTag GetReactionTag() const override;
	
private:
	UPROPERTY()
	class UNpcActivityIndefinetilyStayAtLocationSettings* Settings;
};

UCLASS()
class UNpcActivityIndefinetilyStayAtLocationSettings : public UNpcQuestActivityBaseSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag NpcStateWhenGoingToSpot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FGameplayTag> NpcReactInSpotOptions;
};
