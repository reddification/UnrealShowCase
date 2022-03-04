#pragma once

#include "CoreMinimal.h"
#include "AIBaseReactionComponent.h"
#include "GameplayTagContainer.h"
#include "AI/Data/AITypesGC.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Data/PerceptionData.h"
#include "AIEnemyReactionsComponent.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UAIEnemyReactionsComponent : public UAIBaseReactionComponent
{
    GENERATED_BODY()

public:
    virtual void ReactToSight(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) override;
    virtual void ReactToDamage(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard) override;
    virtual void ReactToSound(AActor* Actor, const FAIStimulus& Stimulus) override;
    virtual void SetCombatMode(EAICombatReason CombatMode);
    virtual void OnTargetKilled(const AActor* Actor) override;

protected:
    virtual void BeginPlay() override;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FGameplayTagQuery IgnoreActorsWithTag;

private:
    FPerceptionData PerceptionData;
    EAICombatReason CombatReason = EAICombatReason::None;
    FDelegateHandle FocusedNpcReactionStateChangedHandle;
    void OnNpcReactionStateChanged(ABaseCharacter* BaseCharacter, const FGameplayTag& GameplayTag, bool bActive);
    
};