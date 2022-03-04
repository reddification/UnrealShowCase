#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Data/Quests/QuestAction.h"
#include "NpcActivityManager.generated.h"

UINTERFACE()
class UNpcActivityManager : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API INpcActivityManager
{
	GENERATED_BODY()

public:
	virtual void Restore(float DeltaSeconds) {}
	virtual class UNpcActivityInstanceBase* GetRunningActivity() { return nullptr; }
	virtual void RunQuestActivity(const FNpcQuestBehaviorDescriptor& BehaviorDescriptor) { }

	virtual bool IsInteractionInterruptionRequired() const { return true; }

	virtual void TryAbortDialogue() const {}
	virtual ENpcActivityType GetRunningActivityType() const { return ENpcActivityType::None; }

	virtual void SetNavigationFilterClass(const TSubclassOf<UNavigationQueryFilter>& CustomNavigationFilter) {}
	virtual void ResetNavigationFilterClass() {}
};
