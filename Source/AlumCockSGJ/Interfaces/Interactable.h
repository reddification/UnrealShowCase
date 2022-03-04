#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE()
class UInteractable : public UInterface
{
    GENERATED_BODY()
};

class ALUMCOCKSGJ_API IInteractable
{
    GENERATED_BODY()

public:
    virtual bool InteractWithPlayer(class APlayerCharacter* Interactor) { return false; }
    virtual void InSight() {}
    virtual void OutOfSight() {}
    virtual void InSightUnderCursor() {}
    virtual void OutOfSightUnderCursor() {}
};
