#pragma once

#include "CoreMinimal.h"
#include "AIBaseActivityComponent.h"
#include "AI/Controllers/AIBaseController.h"
#include "AI/Interfaces/BasicActivityCharacter.h"
#include "Components/ActorComponent.h"
#include "AIBasicActivityComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UAIBasicActivityComponent : public UAIBaseActivityComponent
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;
    virtual void InitializeActivity() override;
};
