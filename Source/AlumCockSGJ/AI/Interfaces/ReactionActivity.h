#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "ReactionActivity.generated.h"

UINTERFACE()
class UReactionActivity : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IReactionActivity
{
	GENERATED_BODY()

public:
	virtual FGameplayTag GetReactionTag() const { return FGameplayTag::EmptyTag; }
};
