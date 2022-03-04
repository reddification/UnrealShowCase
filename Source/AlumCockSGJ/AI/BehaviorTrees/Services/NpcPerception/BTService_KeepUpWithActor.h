#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_KeepUpWithActor.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_KeepUpWithActor : public UBTService
{
	GENERATED_BODY()

	struct FBTServiceMemory
	{
		bool bKeepingUp = false;
		bool bFollowedActorStoppedKey = false;
	};
	
public:
	UBTService_KeepUpWithActor();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTServiceMemory); }
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector KeepUpFlagKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector FollowedActorStoppedKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 0.f, UIMin = 0.f))
	float KeepUpDistance = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin = 1.f, UIMin = 1.f))
	float KeepUpSpeedFactor = 2.f;
};
