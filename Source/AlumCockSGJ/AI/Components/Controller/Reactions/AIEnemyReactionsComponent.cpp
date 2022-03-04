#include "AIEnemyReactionsComponent.h"

#include "GameplayTagAssetInterface.h"
#include "AI/Controllers/AIScissorsController.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"

void UAIEnemyReactionsComponent::BeginPlay()
{
    Super::BeginPlay();
    InitializeNpc();
}

void UAIEnemyReactionsComponent::ReactToSight(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard)
{
    Super::ReactToSight(Actor, Stimulus, Blackboard);
    // if (Stimulus.WasSuccessfullySensed())
    // {
    //     FVector CharacterLocation = ControlledCharacter->GetActorLocation();
    //     if (!PerceptionData.ClosestObservedActor.IsValid()
    //           || (CharacterLocation - Actor->GetActorLocation()).SizeSquared() < (CharacterLocation - PerceptionData.ClosestObservedActor->GetActorLocation()).SizeSquared())
    //     {
    //         if (!IgnoreActorsWithTag.IsEmpty())
    //         {
    //             INpcCharacter* StatefulCharacter = Cast<INpcCharacter>(Actor);
    //             if (StatefulCharacter)
    //             {
    //                 auto RunningReaction = StatefulCharacter->GetRunningReaction();
    //                 if (RunningReaction.IsValid() && IgnoreActorsWithTag.Matches(FGameplayTagContainer(RunningReaction)))
    //                     return;
    //
    //                 auto NpcLogicComponent = StatefulCharacter->GetNpcLogicComponent();
    //                 if (NpcLogicComponent)
    //                 {
    //                     FocusedNpcReactionStateChangedHandle = StatefulCharacter->GetNpcLogicComponent()->NpcReactionStateChangedEvent.AddUObject(this,
    //                         &UAIEnemyReactionsComponent::OnNpcReactionStateChanged);
    //                 }
    //             }
    //         }
    //         
    //         PerceptionData.ClosestObservedActor = Actor;
    //         if (!PerceptionData.MostDamagingActor.IsValid())
    //         {
    //             Blackboard->SetValueAsObject(BB_CurrentTarget, Actor);
    //             SetCombatMode(EAICombatReason::Visual);
    //         }
    //     }
    // }
    // else
    // {
    //     if (Actor && Actor == OwnerController->GetFocusActor())
    //         OwnerController->ClearFocus(EAIFocusPriority::Gameplay);
    //         
    //     PerceptionData.ClosestObservedActor.Reset();
    //     SetCombatMode(EAICombatReason::None);
    //     Blackboard->ClearValue(BB_CurrentTarget);
    //     OwnerController->SetInterestingLocation(Actor->GetActorLocation());
    // }
}

void UAIEnemyReactionsComponent::ReactToDamage(AActor* Actor, const FAIStimulus& Stimulus, UBlackboardComponent* Blackboard)
{
    Super::ReactToDamage(Actor, Stimulus, Blackboard);
    // if (IsValid(Actor))
    // {
    //     auto ShooterCharacter = Cast<ABaseCharacter>(Actor);
    //     if (IsValid(ShooterCharacter) && ShooterCharacter->GetTeam() == ControlledCharacter->GetTeam())
    //         return;
    // }
    // 	
    // if (Stimulus.IsExpired())
    // {
    //     if (Actor && Actor == OwnerController->GetFocusActor())
    //         OwnerController->ClearFocus(EAIFocusPriority::Gameplay);
    //         
    //     PerceptionData.MostDamagingActor.Reset();
    //     PerceptionData.MostDamagingActorDamage = 0.f;
    //
    //     // if damage sense expired but there's active visual target 
    //     if (PerceptionData.ClosestObservedActor.IsValid())
    //     {
    //         OwnerController->SetFocus(PerceptionData.ClosestObservedActor.Get(), EAIFocusPriority::Gameplay);
    //         SetCombatMode(EAICombatReason::Visual);
    //         OwnerController->GetBlackboardComponent()->SetValueAsObject(BB_CurrentTarget, PerceptionData.ClosestObservedActor.Get());
    //     }
    //     else
    //     {
    //         SetCombatMode(EAICombatReason::None);
    //         if (IsValid(Actor))
    //             OwnerController->SetInterestingLocation(Actor->GetActorLocation());
    //     }
    // }
    // else
    // {
    //     if (PerceptionData.MostDamagingActorDamage < Stimulus.Strength)
    //     {
    //         OwnerController->SetFocus(Actor, EAIFocusPriority::Gameplay);
    //         SetCombatMode(EAICombatReason::Defend);
    //         PerceptionData.MostDamagingActor = Actor;
    //         PerceptionData.MostDamagingActorDamage = Stimulus.Strength;
    //         Blackboard->SetValueAsObject(BB_CurrentTarget, Actor);
    //     }
    // }
}

void UAIEnemyReactionsComponent::ReactToSound(AActor* Actor, const FAIStimulus& Stimulus)
{
    Super::ReactToSound(Actor, Stimulus);
    // if (Stimulus.WasSuccessfullySensed())
    // {
    //     OwnerController->SetInterestingLocation(Stimulus.StimulusLocation);
    // }    
}

void UAIEnemyReactionsComponent::SetCombatMode(EAICombatReason CombatMode)
{
    CombatReason = CombatMode;
    if (CombatMode != EAICombatReason::None)
        OwnerController->GetBlackboardComponent()->SetValueAsEnum(BB_CombatMode, (uint8)CombatMode);
    else
        OwnerController->GetBlackboardComponent()->ClearValue(BB_CombatMode);
}

void UAIEnemyReactionsComponent::OnTargetKilled(const AActor* Actor)
{
    OwnerController->ClearFocus(EAIFocusPriority::Gameplay);
    OwnerController->GetBlackboardComponent()->ClearValue(BB_CurrentTarget);
    // if (Actor == PerceptionData.ClosestObservedActor)
    //     PerceptionData.ClosestObservedActor.Reset();
    // if (Actor == PerceptionData.MostDamagingActor)
    //     PerceptionData.MostDamagingActor.Reset();
}

void UAIEnemyReactionsComponent::OnNpcReactionStateChanged(ABaseCharacter* BaseCharacter,
                                                           const FGameplayTag& GameplayTag, bool bActive)
{
    if (bActive && BaseCharacter == OwnerController->GetFocusActor() && BaseCharacter == PerceptionData.ClosestObservedActor)
    {
        if (IgnoreActorsWithTag.Matches(FGameplayTagContainer(GameplayTag)))
        {
            OwnerController->ClearFocus(EAIFocusPriority::Gameplay);
            OwnerController->GetBlackboardComponent()->ClearValue(BB_CurrentTarget);
            PerceptionData.ClosestObservedActor.Reset();
            auto NpcCharacter = Cast<INpcCharacter>(BaseCharacter);
            NpcCharacter->GetNpcLogicComponent()->NpcReactionStateChangedEvent.Remove(FocusedNpcReactionStateChangedHandle);
            FocusedNpcReactionStateChangedHandle.Reset();
        }
    }
}
