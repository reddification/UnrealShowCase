#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "AI/Interfaces/UtilityActivity.h"
#include "NpcActivityWanderAroundInstance.generated.h"

class UNpcActivityWanderAroundSettings;
UCLASS()
class ALUMCOCKSGJ_API UNpcActivityWanderAroundInstance : public UNpcActivityInstanceBase, public ITraversalActivity
{
	GENERATED_BODY()
	
public:
	virtual bool FindNextLocation(const AAIController* AIController, FNextLocationData& NextLocationData) override;
	virtual bool AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard) override;
	
private:
	int LastLocationIndex = -1;
	const UNpcActivityWanderAroundSettings* GetSettings() const;
	mutable const UNpcActivityWanderAroundSettings* WanderAroundSettings;
};
