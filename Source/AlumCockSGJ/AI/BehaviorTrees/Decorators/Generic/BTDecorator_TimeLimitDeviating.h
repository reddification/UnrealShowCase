#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Decorators/BTDecorator_TimeLimit.h"
#include "BTDecorator_TimeLimitDeviating.generated.h"

UCLASS(Category="Generic")
class ALUMCOCKSGJ_API UBTDecorator_TimeLimitDeviating : public UBTDecorator_TimeLimit
{
	GENERATED_BODY()

public:
	virtual void OnNodeActivation(FBehaviorTreeSearchData& SearchData) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin=0.f))
	float RandomDeviation = 0.f;
};
