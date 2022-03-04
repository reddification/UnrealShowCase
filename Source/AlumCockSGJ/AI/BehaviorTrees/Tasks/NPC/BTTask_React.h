#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_React.generated.h"

struct FNpcReactTaskMemory
{
	bool bStarted = false;
	int ReactionIndex = 0;
	FGameplayTag RunningTag;
	float TimeLeft = 0.f;
};

UENUM(BlueprintType)
enum class EAiReactTaskType : uint8
{
	Immediate,
	Locking,
	Indefinite
};

UCLASS(Category = "NPC")
class ALUMCOCKSGJ_API UBTTask_React : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_React();
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bTagFromActivity = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bTagFromActivity"))
	FBlackboardKeySelector ParameterReactionTypeTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bTagFromActivity && !ParameterReactionTypeTag.IsSet()"))
	FGameplayTag ReactionTypeTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OutRecommendedStayTimeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EAiReactTaskType ReactTaskType = EAiReactTaskType::Immediate;

private:
	FGameplayTag GetActualTag(UBehaviorTreeComponent& OwnerComp) const;
};
