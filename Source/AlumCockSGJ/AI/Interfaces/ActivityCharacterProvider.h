#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "UObject/Interface.h"
#include "ActivityCharacterProvider.generated.h"

UINTERFACE()
class UActivityCharacterProvider : public UInterface
{
	GENERATED_BODY()
};

class ALUMCOCKSGJ_API IActivityCharacterProvider
{
	GENERATED_BODY()

public:
	virtual ABaseCharacter* GetActivityCharacter() const { return nullptr; }
};
