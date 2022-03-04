#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_IsBeingAimedAt.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTDecorator_IsBeingAimedAt : public UBTDecorator
{
    GENERATED_BODY()
	
public:
    UBTDecorator_IsBeingAimedAt();

protected:
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FBlackboardKeySelector CharacterKey;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin=0.f, UIMax = 1.f, ClampMax = 1.f))
    float DotProductThreshold = 0.95;
    
    virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
