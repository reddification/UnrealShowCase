#include "NpcActivityGatherItemsAtLocationInstance.h"

#include "NavigationSystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Controllers/AICitizenController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Game/GCGameInstance.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Subsystems/WorldLocationsSubsystem.h"

void UNpcActivityGatherItemsAtLocationInstance::RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard)
{
	Super::RestoreBlackboardStateInternal(Blackboard);
	auto Settings = GetSettings();
	if (Settings)
	{
		float WorldTime = GetOuter()->GetWorld()->GetTimeSeconds();
		bool bRememberHowManyGathered = IsRememberHowManyItemsGathered(WorldTime);
		Blackboard->SetValueAsBool(BB_bKnowActivtyFact1, bRememberHowManyGathered);
		if (bRememberHowManyGathered)
		{
			Blackboard->SetValueAsInt(BB_IterationsCount, bRememberHowManyGathered ? ActivityItemsCountLeft : (int)BB_UnsetInteger);
		}
	}
}

void UNpcActivityGatherItemsAtLocationInstance::OnInteractionActorAvailableStateChanged(
	AActor* InteractiveActor, bool bAvailable)
{
	Super::OnInteractionActorAvailableStateChanged(InteractiveActor, bAvailable);
	if (!bAvailable && InteractiveActor == RememberedActorData.Actor)
		RememberedActorData = FRememberedActorData(); 
}

bool UNpcActivityGatherItemsAtLocationInstance::AssessActivityState(AAIController* AIController, UBlackboardComponent* Blackboard)
{
	if (!bActive)
		return false;
	
	auto Settings = GetSettings();
	TArray<AActor*> CollectedItems = CoreLocation->GetOverlappedActorsInVolume(Settings->ItemType);
	ActivityItemsCountLeft = Settings->RequiredCount - CollectedItems.Num();
	if (ActivityItemsCountLeft <= 0)
		Complete(Blackboard);
	
	RememberHowManyCollectedExpires = AIController->GetWorld()->GetTimeSeconds() + Settings->NumberOfItemsMemoryLifespanSeconds;
	Blackboard->SetValueAsBool(BB_bKnowActivtyFact1, true);
	Blackboard->SetValueAsInt(BB_IterationsCount, ActivityItemsCountLeft);
	// UE_LOG(LogNpcActivity, Log, TEXT("[%s %s %f s] Assessed state:\n\tActivityItemsCountLeft: %d "), *AIController->GetName(),
	// 	*GET_FUNCTION_NAME_CHECKED(UNpcActivityGatherItemsAtLocationInstance, AssessActivityState).ToString(), AIController->GetWorld()->GetTimeSeconds(),
	// 	ActivityItemsCountLeft);
	
	return true;
}

bool UNpcActivityGatherItemsAtLocationInstance::FindNextLocation(const AAIController* AIController,
	FNextLocationData& NextLocationData)
{
	if (!bActive)
		return false;
	
	auto Settings = GetSettings();
	if (IsValid(RememberedActorData.Actor) //&& RememberedActorData.Actor->IsAvailable()
		&& AIController->GetWorld()->GetTimeSeconds() - RememberedActorData.RememberedActorAtWorldTime < Settings->LastObservedActorMemoryLifespanSeconds)
	{
		NextLocationData.NextLocation = RememberedActorData.RememberedActorLocation;
		// UE_LOG(LogNpcActivity, Log, TEXT("[%s %s %f s] Retreived next location from remembered:\n\tOutLocation: %s "), *AIController->GetName(),
		// 	*GET_FUNCTION_NAME_CHECKED(UNpcActivityGatherItemsAtLocationInstance, FindNextLocation).ToString(), AIController->GetWorld()->GetTimeSeconds(),
		// 	*NextLocationData.NextLocation.ToString());
		return true;
	}
	
	if (Settings->LocationsToSearchItems.Num() <= 0)
		return false; // EQS
	
	auto WorldLocationsSubsystem = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WorldLocationsSubsystem)
		return false;

	// TODO refactor the switch case -_-
	FDataTableRowHandle LocationDTRH;
	switch (Settings->LocationsTraversalType)
	{
		case ETraversalType::Random:
			if (Settings->LocationsToSearchItems.Num() <= 2)
				goto sequential_traversal;
			{
				int NextLocationIndex = -1;
				do
					NextLocationIndex = FMath::RandRange(0, Settings->LocationsToSearchItems.Num() - 1);
				while (NextLocationIndex != LastLocationIndex);
			
				LastLocationIndex = NextLocationIndex;
				LocationDTRH = Settings->LocationsToSearchItems[LastLocationIndex]; 
				break;
			}
		case ETraversalType::Sequential:
			sequential_traversal: // лол ну а хули, если нет сахарка из C#
			if (UseEqsRoamCount > 0)
			{
				UseEqsRoamCount--;
				return false;
			}
		
			LocationDTRH = Settings->LocationsToSearchItems[LastLocationIndex];
			break;
		case ETraversalType::Closest:
			// TODO
			break;
		default:
			break;
	}
	
	auto WorldLocation = WorldLocationsSubsystem->GetWorldLocation(LocationDTRH);
	if (!WorldLocation)
		return false;

	NextLocationData.NextLocation = WorldLocation->GetActorLocation();
	return true;
}

void UNpcActivityGatherItemsAtLocationInstance::Observe(const AAIController* AIController, UBlackboardComponent* Blackboard)
{
	const auto ActorInteractionData = GetActorInteractionData();
	if (!bActive || !IsValid(CoreLocation) || ActorInteractionData->IsInteracting())
		return;
	
	TArray<AActor*> ObservedActors;
	AIController->PerceptionComponent->GetKnownPerceivedActors(UAISense_Sight::StaticClass(), ObservedActors);
	if (ObservedActors.Num() <= 0)
		return;

	auto Settings = GetSettings();
	auto IrrelevantActors = CoreLocation->GetOverlappedActorsInVolume(Settings->ItemType);
	float MaxSquaredDistanceToBase = FLT_MAX;
	if (!ActorInteractionData->IsNone())
	{
		MaxSquaredDistanceToBase = FVector::DistSquared(CoreLocation->GetActorLocation(), ActorInteractionData->ActorToInteract->GetActorLocation());
	}
	
	// TODO improve to consider how many actors are spotted in that location IF NPC can, say, carry 2 boxes simultaneously
	bool bNewRememberedActor = false;
	AActor* BestActorToInteract = nullptr; // this is dumb but lets pretend i'm doing this because an actor might be destroyed while NPC is approaching it
	float SqDistanceThreshold = 150.f * 150.f;
	for (const auto ObservedActor : ObservedActors)
	{
		auto CurrentlyInteractedActor = ActorInteractionData->ActorToInteract;
		if (IrrelevantActors.Contains(ObservedActor) || ObservedActor == CurrentlyInteractedActor)
			continue;
		
		float CurrentSquaredDistance = FVector::DistSquared(ObservedActor->GetActorLocation(), AIController->GetPawn()->GetActorLocation());
		if (CurrentSquaredDistance > Settings->MaxItemRecognitionRange * Settings->MaxItemRecognitionRange)
			continue;
		
		float SquareDistanceToBase = FVector::DistSquared(ObservedActor->GetActorLocation(), CoreLocation->GetActorLocation());
		if (MaxSquaredDistanceToBase - SquareDistanceToBase > SqDistanceThreshold && ObservedActor->IsA(Settings->ItemType)
			&& INpcInteractableActor::Execute_IsNpcInteractionAvailable(ObservedActor, Settings->InteractionTag))
		{
			bNewRememberedActor = true;
			BestActorToInteract = ObservedActor;
			MaxSquaredDistanceToBase = CurrentSquaredDistance;
		}
	}

	if (bNewRememberedActor)
		SetInteractionActor(Blackboard, BestActorToInteract);
}

void UNpcActivityGatherItemsAtLocationInstance::OnPerceptionUpdated(AActor* Actor, FAIStimulus AIStimulus)
{
	Super::OnPerceptionUpdated(Actor, AIStimulus);
	if (AIStimulus.IsExpired())
		return;

	FGameplayTag InteractionTag = GetInteractionTag();
	float WorldTime = GetOuter()->GetWorld()->GetTimeSeconds();
	if (Actor == RememberedActorData.Actor)
	{
		if (INpcInteractableActor::Execute_IsNpcInteractionAvailable(Actor, InteractionTag))
		{
			RememberedActorData.SquaredDistanceToBase = FVector::DistSquared(CoreLocation->GetActorLocation(), Actor->GetActorLocation());
			RememberedActorData.RememberedActorAtWorldTime = WorldTime;
			RememberedActorData.RememberedActorLocation = Actor->GetActorLocation(); // in case it was moved 
		}

		return;
	}
	
	auto Settings = GetSettings();
	auto IrrelevantActors = CoreLocation->GetOverlappedActorsInVolume(Settings->ItemType);
	const auto ActorInteractionData = GetActorInteractionData();
	if (IrrelevantActors.Contains(Actor) || Actor == ActorInteractionData->ActorToInteract)
		return;

	float SquaredDistanceToBase = FVector::DistSquared(Actor->GetActorLocation(), CoreLocation->GetActorLocation());
	if (SquaredDistanceToBase > Settings->MaxItemRecognitionRange * Settings->MaxItemRecognitionRange)
		return;

	TSubclassOf<UAISense> StimulusClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetOuter()->GetWorld(), AIStimulus);
	if (StimulusClass != UAISense_Sight::StaticClass())
		return;
	
	float MaxSquaredDistance = FLT_MAX;
	if (IsValid(RememberedActorData.Actor) && WorldTime - RememberedActorData.RememberedActorAtWorldTime < Settings->LastObservedActorMemoryLifespanSeconds)
		MaxSquaredDistance = RememberedActorData.SquaredDistanceToBase;
	
	if (SquaredDistanceToBase < MaxSquaredDistance && Actor->IsA(Settings->ItemType))
	{
		if (INpcInteractableActor::Execute_IsNpcInteractionAvailable(Actor, InteractionTag))
		{
			RememberedActorData.Actor = Actor;
			RememberedActorData.RememberedActorAtWorldTime = WorldTime;
			RememberedActorData.RememberedActorLocation = Actor->GetActorLocation();
			RememberedActorData.SquaredDistanceToBase = SquaredDistanceToBase;
		}
	}
}

bool UNpcActivityGatherItemsAtLocationInstance::StartInteracting(const AAIController* AIController,
	AActor* ActorToInteract, FNpcActivityLatentActionStateChangedEvent* LatentActionCallback, bool bResetActorOnFail)
{
	bool bStartInteracting = Super::StartInteracting(AIController, ActorToInteract, LatentActionCallback, bResetActorOnFail);
	if (!bStartInteracting)
		return false;
	
	// Reset because this actor is reached
	if (ActorToInteract == RememberedActorData.Actor)
		RememberedActorData = FRememberedActorData();
	
	return true;
}

bool UNpcActivityGatherItemsAtLocationInstance::StopInteracting(const AAIController* AIController,
	FNpcActivityLatentActionStateChangedEvent* LatentActionCallback, bool bResetActorOnFail)
{
	bool bCanStopInteracting = Super::StopInteracting(AIController, LatentActionCallback, bResetActorOnFail);
	if (!bCanStopInteracting)
		return false;

	SetActorInteractionDataState(ENpcActorInteractionState::Releasing);
	return true;
}

void UNpcActivityGatherItemsAtLocationInstance::ProgressIteration(int IterationChange)
{
	Super::ProgressIteration(IterationChange);
	auto Settings = GetSettings();
	LastLocationIndex = (LastLocationIndex + IterationChange) % Settings->LocationsToSearchItems.Num();
	if (LastLocationIndex == 0)
		UseEqsRoamCount = Settings->RoamWithEqsOnTraversalEndCount - 1;
}

void UNpcActivityGatherItemsAtLocationInstance::ResetNumberOfItemsMemory()
{
	// idk 
}

void UNpcActivityGatherItemsAtLocationInstance::ResetLastActorMemory()
{
	// idk 
}

const UNpcActivityGatherItemsAtLocationSettings* UNpcActivityGatherItemsAtLocationInstance::GetSettings() const
{
	if (!GatherItemsSettings)
		GatherItemsSettings = Cast<UNpcActivityGatherItemsAtLocationSettings>(ActivitySettings);
		
	return GatherItemsSettings;
}

bool UNpcActivityGatherItemsAtLocationInstance::IsRememberHowManyItemsGathered(float WorldTime) const
{
	return RememberHowManyCollectedExpires > WorldTime;
}
