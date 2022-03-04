#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_RunEQS.h"
#include "BTService_MaintainFollowingAlong.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_MaintainFollowingAlong : public UBTService_RunEQS
{
	GENERATED_BODY()

public:
	UBTService_MaintainFollowingAlong(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector FollowedActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector FollowedActorStoppedFlagKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.1f, ClampMin = 0.1f))
	float PredictionTime = 1.f; 
};