#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIBaseActivityComponent.generated.h"

class AAIBaseController;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UAIBaseActivityComponent : public UActorComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

    virtual void InitializeActivity() {}

    UPROPERTY()
    AAIBaseController* OwnerController;
};
