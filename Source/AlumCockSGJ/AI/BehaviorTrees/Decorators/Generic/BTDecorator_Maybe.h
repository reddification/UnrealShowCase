#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Maybe.generated.h"

UCLASS(Category="Generic")
class ALUMCOCKSGJ_API UBTDecorator_Maybe : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_Maybe();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f))
	float Probability = 0.5;
};
