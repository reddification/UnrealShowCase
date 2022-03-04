#pragma once

#include "CoreMinimal.h"
#include "BTTask_BaseActivityTask.h"
#include "AI/ActivityInstances/NpcActivityInstance.h"
#include "AI/Data/DelegatesAI.h"
#include "BTTask_Interact.generated.h"

UCLASS(Category = "Activities")
class ALUMCOCKSGJ_API UBTTask_Interact : public UBTTask_BaseActivityTask
{
	GENERATED_BODY()

public:
	UBTTask_Interact();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bStartInteract = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector ActorKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bResetInteractionActorOnFail = true;
	
private:
	FNpcActivityLatentActionStateChangedEvent OnInteractionCompleted;
	void OnTaskCompleted(ENpcActivityLatentActionState State);

	UPROPERTY()
	UBehaviorTreeComponent* MyOwnerComp;
};
