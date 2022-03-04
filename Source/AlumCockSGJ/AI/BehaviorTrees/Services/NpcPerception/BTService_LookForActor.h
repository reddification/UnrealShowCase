#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "BehaviorTree/BTService.h"
#include "Decorators/BTDecorator_UtilityBlackboard.h"
#include "BTService_LookForActor.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBTService_LookForActor : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_LookForActor();
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override; 
	virtual FString GetStaticDescription() const override;
	
protected:
	// TODO improve to support array of options for multiclass search?
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClassToLookFor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery WithTagsQuery;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FBlackboardKeySelector OutActorKey;
};
