#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "GameplayTagActor.generated.h"

UINTERFACE()
class UGameplayTagActor : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IGameplayTagActor
{
	GENERATED_BODY()

public:
	virtual void ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd = true) {}
};
