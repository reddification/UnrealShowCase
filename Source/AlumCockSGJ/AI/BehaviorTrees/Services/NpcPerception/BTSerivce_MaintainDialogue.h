#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTSerivce_MaintainDialogue.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTSerivce_MaintainDialogue : public UBTService
{
	GENERATED_BODY()

public:
	UBTSerivce_MaintainDialogue();

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaintainDialogueDistance = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CallInterlocutorAtRange = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector CallInterlocutorFlagKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector FinishDialogueFlagKey;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector InterlocutorKey;
};
