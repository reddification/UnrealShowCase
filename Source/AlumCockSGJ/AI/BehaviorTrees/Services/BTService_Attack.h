#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "BTService_Attack.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_Attack : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Attack();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	// virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxAttackRange = 1000.f;

private:
	// TWeakObjectPtr<class UBlackboardComponent> Blackboard;
	// TWeakObjectPtr<class AAIController> AIController;
	// TWeakObjectPtr<class ABaseHumanoidCharacter> BotCharacter;
};
