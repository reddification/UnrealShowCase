#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIBaseReactionComponent.generated.h"

class AAIScissorsController;
class AAIBaseController;
class ABaseCharacter;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ALUMCOCKSGJ_API UAIBaseReactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	void SetSensesBlocked(bool bBlockFeelings);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	virtual void ReactToSight(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) {}
	virtual void ReactToDamage(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) {}
	virtual void ReactToSound(AActor* Actor, const FAIStimulus& Stimulus) {}

	UPROPERTY()
    AAIBaseController* OwnerController;

    UPROPERTY()
    ABaseCharacter* ControlledCharacter;

    virtual void OnTargetKilled(const AActor* Actor) {}
	virtual void OnPawnChanged();

	virtual void InitializeNpc() {}

	bool bFeelingsBlocked = false;
	
private:
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};