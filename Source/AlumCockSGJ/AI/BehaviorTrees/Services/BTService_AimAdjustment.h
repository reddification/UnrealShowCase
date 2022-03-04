#pragma once

#include "CoreMinimal.h"
#include "CommonConstants.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AimAdjustment.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_AimAdjustment : public UBTService
{
	GENERATED_BODY()
public:
    UBTService_AimAdjustment();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FBlackboardKeySelector CharacterKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName CharacterTargetSocket = AISocketDefaultCharacterTarget;
    
    virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
