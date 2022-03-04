#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "UObject/Object.h"
#include "BTTask_PredictFollowedActorLocation.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_PredictFollowedActorLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_PredictFollowedActorLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector FollowedActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.1f, UIMin = 0.1f))
	float PredictionTime = 1.f;
};
