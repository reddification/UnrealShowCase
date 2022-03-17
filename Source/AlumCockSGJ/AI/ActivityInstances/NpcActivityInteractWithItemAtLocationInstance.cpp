#include "NpcActivityInteractWithItemAtLocationInstance.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Actors/Interactive/NpcInteraction/NpcBaseInteractiveActor.h"
#include "AI/Interfaces/BlackboardProvider.h"
#include "Characters/BaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Subsystems/WorldLocationsSubsystem.h"

bool UNpcActivityInteractWithItemAtLocationInstance::AssessActivityState(AAIController* AIController,
	UBlackboardComponent* Blackboard)
{
	TArray<AActor*> ObservedActors;
	AIController->GetPerceptionComponent()->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), ObservedActors);
	FGameplayTag InteractionTag = GetInteractionTag();
	auto Settings = GetCurrentActivitySettings();
	auto InteractionPlan = Settings->InteractionsSchedule[CurrentInteractionIndex];
	for (const auto Actor : ObservedActors)
	{
		if (Actor->IsA(InteractionPlan.InteractiveActorClass))
		{
			if (!INpcInteractableActor::Execute_IsNpcInteractionAvailable(Actor, InteractionTag))
				continue;
			
			auto BlackboardProvider = Cast<IBlackboardProvider>(GetOuter());
			SetInteractionActor(BlackboardProvider->GetBlackboard(), Actor);
			return true;
		}
	}
	
	return Super::AssessActivityState(AIController, Blackboard);
}

bool UNpcActivityInteractWithItemAtLocationInstance::FindNextLocation(const AAIController* AIController, FNextLocationData& OutNextLocationData)
{
	auto WLS = GetOuter()->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WLS)
		return false;
	
	auto Settings = GetCurrentActivitySettings();
	if (Settings->bRandom && Settings->InteractionsSchedule.Num() > 2)
	{
		int NextInteractionIndex = CurrentInteractionIndex;
		do
			NextInteractionIndex = FMath::RandRange(0, Settings->InteractionsSchedule.Num() - 1);
		while (NextInteractionIndex == CurrentInteractionIndex);
		CurrentInteractionIndex = NextInteractionIndex;
	}
	else
	{
		CurrentInteractionIndex	 = (CurrentInteractionIndex + 1) % Settings->InteractionsSchedule.Num();
	}

	auto InteractionPlan = Settings->InteractionsSchedule[CurrentInteractionIndex];
	auto WorldLocation = WLS->GetClosestQuestLocationComplex(InteractionPlan.WorldLocationDTRH,
		AIController->GetPawn()->GetActorLocation(), GetOuter());
	if (!WorldLocation)
		return false;

	OutNextLocationData.NextLocation = WorldLocation->GetActorLocation();
	OutNextLocationData.RecommendedStayTime = InteractionPlan.Duration;
	return true;
}

void UNpcActivityInteractWithItemAtLocationInstance::Observe(const AAIController* AIController,
															 UBlackboardComponent* Blackboard)
{
	const auto ActorInteractionData = GetActorInteractionData();
	if (!bActive || ActorInteractionData->IsApproaching())
		return;
	
	TArray<AActor*> ObservedActors;
	AIController->PerceptionComponent->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), ObservedActors);
	
	auto Settings = GetCurrentActivitySettings();
	auto InteractionPlan = Settings->InteractionsSchedule[CurrentInteractionIndex];
	float MaxSquaredDistance = FLT_MAX;
	AActor* BestActorToInteract = nullptr;
	FGameplayTag InteractionTag = GetInteractionTag();
	for (const auto ObservedActor : ObservedActors)
	{
		if (ObservedActor->IsA(InteractionPlan.InteractiveActorClass))
		{
			if (!INpcInteractableActor::Execute_IsNpcInteractionAvailable(ObservedActor, InteractionTag))
				continue;
			
			float CurrentSquaredDistance = FVector::DistSquared(AIController->GetPawn()->GetActorLocation(), ObservedActor->GetActorLocation());
			if (CurrentSquaredDistance < MaxSquaredDistance)
			{
				MaxSquaredDistance = CurrentSquaredDistance;
				BestActorToInteract = ObservedActor;
			}
		}
	}

	if (BestActorToInteract)
		SetInteractionActor(Blackboard, BestActorToInteract);
}

bool UNpcActivityInteractWithItemAtLocationInstance::StartInteracting(const AAIController* AIController,
	AActor* ActorToInteract, FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetActorOnFail)
{
	bool bInteractionStarted = Super::StartInteracting(AIController, ActorToInteract, InteractionCompletedCallback, bResetActorOnFail);
	if (!bInteractionStarted)
		return false;
	
	auto Settings = GetCurrentActivitySettings();
	auto InteractionPlan = Settings->InteractionsSchedule[CurrentInteractionIndex];
	GetOuter()->GetWorld()->GetTimerManager().SetTimer(InteractionTimer, this,
		&UNpcActivityInteractWithItemAtLocationInstance::OnInteractionCompleted, InteractionPlan.Duration * FMath::RandRange(0.8f, 1.2f));
	
	return true;
}

void UNpcActivityInteractWithItemAtLocationInstance::OnInteractionCompleted()
{
	StopInteracting(Cast<AAIController>(GetOuter()), ActiveLatentActionCallback, false);
}

const UNpcActivityInteractWithItemSettings* UNpcActivityInteractWithItemAtLocationInstance::GetCurrentActivitySettings() const
{
	if (!CurrentActivitySettings)
		CurrentActivitySettings = Cast<UNpcActivityInteractWithItemSettings>(ActivitySettings);

	return CurrentActivitySettings;
}

float UNpcActivityInteractWithItemAtLocationInstance::Suspend(AAIController* AIController, bool bAbortInteraction)
{
	GetOuter()->GetWorld()->GetTimerManager().ClearTimer(InteractionTimer);
	return Super::Suspend(AIController, bAbortInteraction);
}

void UNpcActivityInteractWithItemAtLocationInstance::ProgressIteration(int IterationChange)
{
	Super::ProgressIteration(IterationChange);
	auto Settings = GetCurrentActivitySettings();
	CurrentInteractionIndex = (CurrentInteractionIndex + IterationChange) % Settings->InteractionsSchedule.Num();
}

void UNpcActivityInteractWithItemAtLocationInstance::OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus)
{
	Super::OnPerceptionUpdated(Actor, AIStimulus);
	const auto ActorInteractionData = GetActorInteractionData();
	if (!bActive || ActorInteractionData->IsApproaching())
		return;
	
	auto Settings = GetCurrentActivitySettings();
	auto InteractionPlan = Settings->InteractionsSchedule[CurrentInteractionIndex];
	if (AIStimulus.IsExpired() || UAIPerceptionSystem::GetSenseClassForStimulus(Actor, AIStimulus) != UAISense_Sight::StaticClass())
		return;

	FGameplayTag InteractionTag = GetInteractionTag();
	if (Actor->IsA(InteractionPlan.InteractiveActorClass))
	{
		if (!INpcInteractableActor::Execute_IsNpcInteractionAvailable(Actor, InteractionTag))
			return;

		auto BlackboardProvider = Cast<IBlackboardProvider>(GetOuter());
		if (BlackboardProvider)
			SetInteractionActor(BlackboardProvider->GetBlackboard(), Actor);
	}
}
