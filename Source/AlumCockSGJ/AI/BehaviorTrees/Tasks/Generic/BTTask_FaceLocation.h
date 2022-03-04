#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FaceLocation.generated.h"

struct FLookAtNodeMemory
{
	FVector LookAtLocation;
	float PrecisionDot = 0.f;
};

UCLASS()
class ALUMCOCKSGJ_API UBTTask_FaceLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FaceLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FLookAtNodeMemory); }
	virtual FString GetStaticDescription() const override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector LookAtLocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float PrecisionDegrees = 0.f;

private:
	float CalculateAngleDifferenceDot(const FVector& VectorA, const FVector& VectorB);
};
