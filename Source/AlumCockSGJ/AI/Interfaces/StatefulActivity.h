#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "StatefulActivity.generated.h"

UINTERFACE()
class UStatefulActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IStatefulActivity
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetStateTag() const { return FGameplayTag::EmptyTag; }
};
