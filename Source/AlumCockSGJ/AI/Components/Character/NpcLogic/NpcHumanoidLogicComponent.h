#pragma once

#include "CoreMinimal.h"
#include "NpcBaseLogicComponent.h"
#include "NpcHumanoidLogicComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UNpcHumanoidLogicComponent : public UNpcBaseLogicComponent
{
	GENERATED_BODY()

public:
	virtual void ApplyState(const UBaseNpcStateSettings* NpcState) override;
};
