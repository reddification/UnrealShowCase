#pragma once

#include "CoreMinimal.h"
#include "BasePlatform.h"
#include "GameFramework/Actor.h"
#include "TriggerablePlatform.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ATriggerablePlatform : public ABasePlatform
{
	GENERATED_BODY()

protected:
	virtual void OnPlatformReachedFinalPosition() override;
};
