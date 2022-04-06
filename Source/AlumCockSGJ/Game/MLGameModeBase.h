#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MLGameModeBase.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AMLGameModeBase : public AGameMode
{
    GENERATED_BODY()
    
protected:
    virtual void StartPlay() override;
};
