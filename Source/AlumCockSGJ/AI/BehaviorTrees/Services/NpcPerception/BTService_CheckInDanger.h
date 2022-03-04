#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "UObject/Object.h"
#include "BTService_CheckInDanger.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_CheckInDanger : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CheckInDanger();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OutKey;
};
