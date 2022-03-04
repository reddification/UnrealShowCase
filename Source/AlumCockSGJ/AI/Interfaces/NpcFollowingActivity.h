#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NpcFollowingActivity.generated.h"

UINTERFACE()
class UNpcFollowingActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API INpcFollowingActivity
{
	GENERATED_BODY()

public:
	virtual void SetFollowedActor(APawn* FollowedActor) {}
	virtual void SetPredictedLocation(FVector ExpectedLocation) {}
	virtual APawn* GetFollowedActor() const { return nullptr; }
	virtual const FVector& GetPredictedLocation() const { return FVector::ZeroVector; }
	virtual bool IsFollowedActorStopped() const { return false; }
	virtual void SetFollowedActorStopped(bool bNewValue) {  } 
};
