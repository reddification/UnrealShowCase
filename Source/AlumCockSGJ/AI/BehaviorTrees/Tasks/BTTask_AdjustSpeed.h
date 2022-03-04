#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AdjustSpeed.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_AdjustSpeed : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AdjustSpeed();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetLocationKey;

	// TODO put it inside FollowingActivity DA settings to keep it synchronized between predicting location and adjusting speed
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.1f, UIMin = 0.1f))
	float PredictionTime = 1.f;
};
