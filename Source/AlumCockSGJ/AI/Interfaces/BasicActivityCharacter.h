#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BasicActivityCharacter.generated.h"

UINTERFACE()
class UBasicActivityCharacter : public UInterface
{
    GENERATED_BODY()
};

class ALUMCOCKSGJ_API IBasicActivityCharacter
{
    GENERATED_BODY()

public:
    virtual class UBehaviorTree* GetDefaultBehaviorTree() const { return nullptr; }
};
