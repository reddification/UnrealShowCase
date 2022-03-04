#pragma once

#include "CoreMinimal.h"
#include "NpcQuestActivityInstance.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "NpcActivityGoToLocationInstance.generated.h"

class UNpcActivityGoToLocationSettings;

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityGoToLocationInstance : public UNpcQuestActivityInstance, public ITraversalActivity
{
	GENERATED_BODY()

public:
	virtual void Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController) override;
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData) override;

private:
	UPROPERTY()
	UNpcActivityGoToLocationSettings* QuestSettings;
};

UCLASS()
class UNpcActivityGoToLocationSettings : public UNpcQuestActivityBaseSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="QuestLocationDTR"))
	TArray<FDataTableRowHandle>	GoToLocations;
};



