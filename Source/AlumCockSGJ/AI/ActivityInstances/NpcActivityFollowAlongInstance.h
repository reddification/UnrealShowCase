#pragma once

#include "CoreMinimal.h"
#include "NpcActivityInstance.h"
#include "AI/Interfaces/NpcFollowingActivity.h"
#include "Quest/NpcQuestActivityInstance.h"
#include "NpcActivityFollowAlongInstance.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UNpcActivityFollowAlongInstance : public UNpcQuestActivityInstance, public INpcFollowingActivity
{
	GENERATED_BODY()

public:
	virtual void Resume() override;
	virtual void Suspend(AAIController* AIController, bool bAbortInteraction) override;

	virtual APawn* GetFollowedActor() const override { return FollowedActor; }
	virtual const FVector& GetPredictedLocation() const override { return PredictedLocation; }
	virtual bool IsFollowedActorStopped() const override { return bFollowedActorStopped; }

	virtual void SetFollowedActorStopped(bool bNewValue) override { bFollowedActorStopped = bNewValue; }
	virtual void SetFollowedActor(APawn* FollowedActor) override;
	virtual void SetPredictedLocation(FVector NewPredictedLocation) override;
	
private:
	FVector PredictedLocation = FAISystem::InvalidLocation;
	bool bFollowedActorStopped = false;
	
	UPROPERTY()
	APawn* FollowedActor = nullptr;
};
