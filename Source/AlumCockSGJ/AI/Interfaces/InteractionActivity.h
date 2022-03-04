#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AI/Data/DelegatesAI.h"
#include "UObject/Interface.h"
#include "InteractionActivity.generated.h"

UINTERFACE()
class UInteractionActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IInteractionActivity
{
	GENERATED_BODY()

public:
	virtual bool StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
		FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetInteractionActorOnFail) { return false; }
	virtual bool StopInteracting(const AAIController* AIController, FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback,
		bool bResetInteractionActorOnFail) { return false; }
};
