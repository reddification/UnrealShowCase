#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_ResetFocusedCharacter.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTTask_ResetFocusedCharacter : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_ResetFocusedCharacter();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector CharacterBlackboardKey;
};