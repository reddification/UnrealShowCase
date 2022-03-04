#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SeeLocation.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_SeeLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_SeeLocation();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector LocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ObservationSuccessFlagKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin = 0.f))
	float RecognizeDistance = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin=0.f, ClampMin = 0.f, UIMax = 1, ClampMax = 1))
	float RecognizeTraceDotProductThreshold = 0.9;
};
