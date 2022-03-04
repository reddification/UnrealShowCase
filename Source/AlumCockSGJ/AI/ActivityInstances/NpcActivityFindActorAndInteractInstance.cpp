#include "NpcActivityFindActorAndInteractInstance.h"

#include "CommonConstants.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"

FInteractionEqsSettings UNpcActivityFindActorAndInteractInstance::PrepareEqsInteractionSettings()
{
	ClearNotIgnoredAnymoreActors();
	auto Settings = GetRestSettings();
	if (CurrentInteractionOptionIndex < 0)
		CurrentInteractionOptionIndex = FMath::RandRange(0, Settings->InteractionOptions.Num() - 1);
	
	const auto& InteractionOption = Settings->InteractionOptions[CurrentInteractionOptionIndex];
	FInteractionEqsSettings InteractionEqsSettings;
	InteractionEqsSettings.IgnoredActors = &IgnoredActors;
	InteractionEqsSettings.GameplayTagQuery = &InteractionOption.ActorTagQuery;
	InteractionEqsSettings.SearchedActorClasses = &InteractionOption.ActorsToInteractWith;
	return InteractionEqsSettings;
}

void UNpcActivityFindActorAndInteractInstance::SetActorToIgnore(AActor* Actor)
{
	ClearNotIgnoredAnymoreActors();
	auto Settings = GetRestSettings();
	FActivityIgnoreActorData NewIgnoredActor;
	NewIgnoredActor.Actor = Actor;
	NewIgnoredActor.UntilWorldTime = GetOuter()->GetWorld()->GetTimeSeconds() + Settings->IgnoreActorWhenUnavailableTime;
	IgnoredActors.Add(NewIgnoredActor);
}

// i don't remember why i even need this lol
bool UNpcActivityFindActorAndInteractInstance::AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard)
{
	Super::AssessActivityState(AIController, Blackboard);
	auto ActorToInteract = Cast<AActor>(Blackboard->GetValueAsObject(BB_ActorToInteract));
	if (!IsValid(ActorToInteract))
	{
		CurrentInteractionOptionIndex = -1;
		ReportActivityEvent(EActivityEventType::Inoperable);
		return false;
	}
	
	SetInteractionActor(Blackboard, ActorToInteract);
	return true;
}

bool UNpcActivityFindActorAndInteractInstance::StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
                                        FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetActorOnFail)
{
	auto bCanStartInteract = Super::StartInteracting(AIController, ActorToInteract, InteractionCompletedCallback, bResetActorOnFail);
	if (!bCanStartInteract || CurrentInteractionOptionIndex < 0)
	{
		CurrentInteractionOptionIndex = -1;
		return false;
	}
	
	auto Settings = GetRestSettings();
	const auto& CurrentInteractionOption = Settings->InteractionOptions[CurrentInteractionOptionIndex];
	float InteractionDuration = FMath::RandRange(CurrentInteractionOption.InteractionDurationTimeMin, CurrentInteractionOption.InteractionDurationTimeMax);
	GetOuter()->GetWorld()->GetTimerManager().SetTimer(InteractionTimer, this,
		&UNpcActivityFindActorAndInteractInstance::OnInteractionCompleted,  InteractionDuration);

	return true;
}

void UNpcActivityFindActorAndInteractInstance::OnInteractionCompleted()
{
	StopInteracting(Cast<AAIController>(GetOuter()), ActiveLatentActionCallback, false);
	CurrentInteractionOptionIndex = -1;
}

// Interaction actor might have been unavailable even before tracking
// because EQS deliberately doesn't filter this as an NPC can't really know if say a bench is fully occupied or not before actually seeing it
void UNpcActivityFindActorAndInteractInstance::OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus)
{
	Super::OnPerceptionUpdated(Actor, AIStimulus);
	const auto ActorInteractionData = GetActorInteractionData();
	if (!(bActive && ActorInteractionData->IsApproaching() && ActorInteractionData->ActorToInteract == Actor && !AIStimulus.IsExpired()))
		return;

	FGameplayTag InteractionTag = GetInteractionTag();
	if (UAIPerceptionSystem::GetSenseClassForStimulus(GetOuter(), AIStimulus) == UAISense_Sight::StaticClass())
	{
		if (!INpcInteractableActor::Execute_IsNpcInteractionAvailable(ActorInteractionData->ActorToInteract, InteractionTag))
		{
			SetActorToIgnore(ActorInteractionData->ActorToInteract);
			ResetInteractionActor();
		}
	}
}

bool UNpcActivityFindActorAndInteractInstance::StopInteracting(const AAIController* AIController,
	FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetInteractionActorOnFail)
{
	auto ActorInteractionData = GetActorInteractionData();
	if (ActorInteractionData->IsInteracting())
		SetActorToIgnore(ActorInteractionData->ActorToInteract);

	return Super::StopInteracting(AIController, InteractionCompletedCallback, bResetInteractionActorOnFail);
}

void UNpcActivityFindActorAndInteractInstance::OnInteractionActorAvailableStateChanged(AActor* InteractiveActor,
                                                               bool bAvailable)
{
	const auto ActorInteractionData = GetActorInteractionData();
	if (ActorInteractionData->IsApproaching() && ActorInteractionData->ActorToInteract == InteractiveActor)
		SetActorToIgnore(InteractiveActor);
	
	Super::OnInteractionActorAvailableStateChanged(InteractiveActor, bAvailable);
}

void UNpcActivityFindActorAndInteractInstance::RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard)
{
	Super::RestoreBlackboardStateInternal(Blackboard);
	Blackboard->ClearValue(BB_ActorToInteract);
}

const UNpcActivityFindActorAndInteractSettings* UNpcActivityFindActorAndInteractInstance::GetRestSettings() const 
{
	if (!RestSettings)
		RestSettings = Cast<UNpcActivityFindActorAndInteractSettings>(GetActivitySettings());

	return RestSettings;
}
