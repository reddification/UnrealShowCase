#pragma once

#include "CoreMinimal.h"
#include "AI/Data/AITypesGC.h"
#include "BehaviorTree/BTService.h"
#include "Characters/BaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "BTService_EnemyBehavior.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_EnemyBehavior : public UBTService
{
	GENERATED_BODY()

struct FBTSeviceMemory
{
	FVector InterestingLocation = FVector::ZeroVector;
};
	
public:
	UBTService_EnemyBehavior();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override { return sizeof(FBTSeviceMemory); }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector CurrentTargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector InterestingLocationKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector CombatModeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery IgnoreActorsWithTag;

private:
	bool IsSensingDamage(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent, UBlackboardComponent* BlackboardComponent,
						 FBTSeviceMemory* ServiceMemory, UObject* CurrentTarget, EAICombatReason CurrentCombatMode);
	bool IsSensingVisual(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent, UBlackboardComponent* BlackboardComponent,
						 FBTSeviceMemory* ServiceMemory, UObject* CurrentTarget, EAICombatReason CurrentCombatMode);
	bool IsSensingSound(AAIController* AIController, UAIPerceptionComponent* PerceptionComponent,
						UBlackboardComponent* BlackboardComponent, FBTSeviceMemory* ServiceMemory,
						UObject* CurrentTarget,
						EAICombatReason CurrentCombatMode);
	bool IgnoreTarget(ABaseCharacter* Target);
	
};
