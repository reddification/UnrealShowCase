#pragma once

#include "CoreMinimal.h"
#include "AI/Data/NpcActorInteractionData.h"
#include "UObject/Interface.h"
#include "NpcInteractionManager.generated.h"

UINTERFACE()
class UNpcInteractionManager : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API INpcInteractionManager
{
	GENERATED_BODY()

public:
	virtual const FNpcActorInteractionData* GetCurrentNpcInteractionData() const { return nullptr; }
	virtual void SetCurrentNpcInteractionData(AActor* Actor, ENpcActorInteractionState State, uint32 ActivityId) {}
	virtual void SetCurrentNpcInteractionDataState(uint32 ActivityId, ENpcActorInteractionState State) {}
	virtual void ResetActorInteractionData() {}
	virtual void ProlongInteractionMemory(float ProlongationTime = 300.f) {}
};
