#include "AIBaseReactionComponent.h"

#include "AIController.h"
#include "CommonConstants.h"
#include "AI/Controllers/AIScissorsController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"
#include "Interfaces/Killable.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Team.h"
#include "Perception/AISense_Touch.h"

void UAIBaseReactionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerController = StaticCast<AAIBaseController*>(GetOwner());
	OwnerController->ControllerPawnResetEvent.AddUObject(this, &UAIBaseReactionComponent::OnPawnChanged);
	OwnerController->PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &UAIBaseReactionComponent::OnTargetPerceptionUpdated);
	OwnerController->TargetKilledEvent.AddUObject(this, &UAIBaseReactionComponent::OnTargetKilled);
	OnPawnChanged();
}

void UAIBaseReactionComponent::SetSensesBlocked(bool bBlockFeelings)
{
	bFeelingsBlocked = bBlockFeelings;
}

void UAIBaseReactionComponent::OnPawnChanged()
{
	ControlledCharacter = Cast<ABaseCharacter>(OwnerController->GetPawn());
	if (ControlledCharacter)
		InitializeNpc();
}

void UAIBaseReactionComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (bFeelingsBlocked || !IsValid(ControlledCharacter))
		return;
	
	auto Blackboard = OwnerController->GetBlackboardComponent();
    if (Blackboard == NULL)
        return;

	IKillable* KillableActor = Cast<IKillable>(Actor);
	if (KillableActor && !KillableActor->IsAlive())
		return;
	
    TSubclassOf<UAISense> StimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
    if (StimulusClass == UAISense_Damage::StaticClass() && Actor != ControlledCharacter)
    	ReactToDamage(Actor, Stimulus, Blackboard);
    else if (StimulusClass == UAISense_Sight::StaticClass())
        ReactToSight(Actor, Stimulus, Blackboard);
    else if (StimulusClass == UAISense_Hearing::StaticClass() && Actor != ControlledCharacter)
        ReactToSound(Actor, Stimulus);
    else if (StimulusClass == UAISense_Touch::StaticClass())
    {
	    if (Stimulus.WasSuccessfullySensed())
	    	UE_LOG(LogTemp, Log, TEXT("AI touch sense?"));
    }
    else if (StimulusClass == UAISense_Team::StaticClass())
    {
        if (Stimulus.WasSuccessfullySensed())
	        UE_LOG(LogTemp, Log, TEXT("AI sees team member"));
    }
}
