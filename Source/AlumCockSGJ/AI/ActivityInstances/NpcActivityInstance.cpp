#include "NpcActivityInstance.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Controllers/AICitizenController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Game/GCGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "AI/Interfaces/NpcInteractionManager.h"
#include "Subsystems/WorldLocationsSubsystem.h"

#pragma region CONTROL

void UNpcActivityInstanceBase::Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor, AAIController* AIController)
{
	ActivitySettings = NpcBehaviorDescriptor.NpcActivitySettings;
	if (!ActivitySettings->BaseActivityLocationDTRH.IsNull())
	{
		auto WLS = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
		if (WLS)
			CoreLocation = WLS->GetWorldLocation(ActivitySettings->BaseActivityLocationDTRH);
	}

	NpcInteractionManager = Cast<INpcInteractionManager>(GetOuter());
}

void UNpcActivityInstanceBase::Resume()
{
	if (bActive)
		return;

	auto ActivityManager = Cast<INpcActivityManager>(GetOuter());
	if (ActivitySettings->ActivityNavigationFilterClass)
		ActivityManager->SetNavigationFilterClass(ActivitySettings->ActivityNavigationFilterClass);

	// TODO requires polishing
	const auto ActorInteractionData = GetActorInteractionData();
	float WorldTime = GetOuter()->GetWorld()->GetTimeSeconds();
	auto Blackboard = Cast<IBlackboardProvider>(GetOuter())->GetBlackboard();
	bool bInteractionRecovered = false;
	if (ActorInteractionData->ActivityId == GetActivityId() && !ActorInteractionData->IsReleasing() && ActorInteractionData->RememberUntilWorldTime > WorldTime)
	{
		Blackboard->SetValueAsObject(BB_ActorToInteract, ActorInteractionData->ActorToInteract);
		Blackboard->SetValueAsEnum(BB_ActorInteractionState, (uint8)ActorInteractionData->State);
		if (!ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.IsBoundToObject(this))
		{
			InteractionDelegateHandle = ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.AddUObject(this,
				&UNpcActivityInstanceBase::OnInteractionStateChanged);
		}
		// SetInteractionActor(Blackboard, ActorInteractionData->ActorToInteract, ActorInteractionData->State);// ???
		bInteractionRecovered = true;
	}
	else if (InteractionMemory.ActorToInteract && InteractionMemory.UntilWorldTime > WorldTime)
	{
		if (INpcInteractableActor::Execute_IsNpcInteractionAvailable(InteractionMemory.ActorToInteract, FGameplayTag::EmptyTag))
		{
			SetInteractionActor(Blackboard, InteractionMemory.ActorToInteract);
			bInteractionRecovered = true;
		}
	}

	if (!bInteractionRecovered)
	{
		Blackboard->ClearValue(BB_ActorToInteract);
		Blackboard->ClearValue(BB_ActorInteractionState);
	}
	
	// TODO include restore BB state here?
	SetActive(true);
}

float UNpcActivityInstanceBase::Suspend(AAIController* AIController, bool bAbortInteraction)
{
	float SuspendDelay = -1.f;
	if (!bActive)
		return SuspendDelay;

	SuspendDelay = 0.f;
	if (bAbortInteraction)
	{
		const auto ActorInteractionData = GetActorInteractionData();
		if (ActorInteractionData->State != ENpcActorInteractionState::None)
		{
			// LastActorInteractionLocation = ActorInteractionData->ActorToInteract->GetActorLocation();
			if (ActorInteractionData->IsInteracting())
				SuspendDelay = StopInteracting(AIController, ActiveLatentActionCallback, true);
			else
				ResetInteractionActor();

			InteractionMemory.ActorToInteract = ActorInteractionData->ActorToInteract;
			InteractionMemory.UntilWorldTime = GetOuter()->GetWorld()->GetTimeSeconds() + 180.f;
		}
	}
	else
	{
		ActiveLatentActionCallback = nullptr;
		ProlongInteractionMemory();
	}

	auto ActivityManager = Cast<INpcActivityManager>(GetOuter());
	if (ActivitySettings->ActivityNavigationFilterClass)
		ActivityManager->ResetNavigationFilterClass();
	
	SetActive(false);
	return SuspendDelay;
}

void UNpcActivityInstanceBase::Complete(UBlackboardComponent* BlackboardComponent)
{
	NpcActivityEvent.ExecuteIfBound(EActivityEventType::Completed);
	SetActive(false);
}

void UNpcActivityInstanceBase::SetActive(bool bActiveNew)
{
	bActive = bActiveNew;
	if (NpcActivityStateChangedEvent.IsBound())
		NpcActivityStateChangedEvent.Broadcast(this, bActiveNew);
}

#pragma endregion CONTROL

#pragma region BLACKBOARD

void UNpcActivityInstanceBase::RestoreBlackboardState(UBlackboardComponent* Blackboard)
{
	if (!ActivitySettings)
		return;

	RestoreBlackboardStateInternal(Blackboard);
}

void UNpcActivityInstanceBase::RestoreBlackboardStateInternal(UBlackboardComponent* Blackboard)
{
	auto WLS = GetOuter()->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WLS)
		return;
	
	Blackboard->SetValueAsObject(BB_ActivityCoreLocationVector, WLS->GetWorldLocation(ActivitySettings->BaseActivityLocationDTRH));
	const auto ActorInteractionData = GetActorInteractionData();
	if (!ActorInteractionData->IsNone())
	{
		if (ActorInteractionData->ActivityId != GetUniqueID())
		{
			auto NpcCharacter = Cast<IActivityCharacterProvider>(GetOuter())->GetActivityCharacter();;
			if (ActorInteractionData->IsInteracting())
				INpcInteractableActor::Execute_StopNpcInteraction(ActorInteractionData->ActorToInteract, NpcCharacter, true);
		
			ClearActorInteractionData();
		}
		else
		{
			Blackboard->SetValueAsObject(BB_ActorToInteract, ActorInteractionData->ActorToInteract);
			Blackboard->SetValueAsEnum(BB_ActorInteractionState, (uint8)ActorInteractionData->State);
		}
	}
	
}

#pragma endregion BLACKBOARD

void UNpcActivityInstanceBase::ReportActivityEvent(EActivityEventType ActivityEventType)
{
	NpcActivityEvent.ExecuteIfBound(ActivityEventType);
}

#pragma region OBSERVATION

void UNpcActivityInstanceBase::StartObserving(AAIController* AIController)
{
	AIController->GetPerceptionComponent()->OnTargetPerceptionUpdated.RemoveDynamic(this, &UNpcActivityInstanceBase::OnPerceptionUpdated);
	AIController->GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &UNpcActivityInstanceBase::OnPerceptionUpdated);
	// auto PerceptionComponent = AIController->GetPerceptionComponent();
	// if (!PerceptionComponent->OnTargetPerceptionUpdated.IsAlreadyBound(this, GET_FUNCTION_NAME_CHECKED(UNpcActivityInstanceBase, OnPerceptionUpdated)))
	// {
	// 	AIController->GetPerceptionComponent()->OnTargetPerceptionUpdated.RemoveDynamic(this, &UNpcActivityInstanceBase::OnPerceptionUpdated);
	// 	AIController->GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &UNpcActivityInstanceBase::OnPerceptionUpdated);
	// }
}

void UNpcActivityInstanceBase::StopObserving(AAIController* AIController)
{
	AIController->GetPerceptionComponent()->OnTargetPerceptionUpdated.RemoveDynamic(this, &UNpcActivityInstanceBase::OnPerceptionUpdated);
}

#pragma endregion OBSERVATION

#pragma region INTERACTION

bool UNpcActivityInstanceBase::StartInteracting(const AAIController* AIController, AActor* ActorToInteract,
	FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback, bool bResetInteractionActorOnFail)
{
	if (!bActive)
		return false;

	auto NpcCharacter = Cast<ABaseCharacter>(AIController->GetPawn());
	if (!NpcCharacter)
		return false;
	
	const auto ActorInteractionData = GetActorInteractionData();
	
	if (ActorInteractionData->IsInteracting())
	{
		ActiveLatentActionCallback = InteractionCompletedCallback;
		return true;
	}
	
	if (ActorInteractionData->IsNone())
		SetActorInteractionData(ActorToInteract, ENpcActorInteractionState::Approaching);
	
	auto CurrentInteractionActor = Cast<INpcInteractableActor>(ActorToInteract);
	if (!CurrentInteractionActor)
		return false;

	PrepareForInteraction();
	FGameplayTag InteractionTag = GetInteractionTag();
	bool bInteractionStarted = INpcInteractableActor::Execute_TryStartNpcInteraction(ActorToInteract, NpcCharacter, InteractionTag);
	if (!bInteractionStarted)
	{
		if (bResetInteractionActorOnFail)
			ResetInteractionActor();
		
		return false;
	}

	ActiveLatentActionCallback = InteractionCompletedCallback;
	SetActorInteractionData(ActorToInteract, ENpcActorInteractionState::Interacting);
	return true;
}

float UNpcActivityInstanceBase::StopInteracting(const AAIController* AIController,
                                                FNpcActivityLatentActionStateChangedEvent* InteractionCompletedCallback,
                                                bool bResetInteractionActorOnFail)
{
	auto ActorInteractionData = GetActorInteractionData();
	float SuspendDelay = StopInteracting(ActorInteractionData);

	if (SuspendDelay >= 0.f)
		ActiveLatentActionCallback = InteractionCompletedCallback;
	
	return SuspendDelay;
}

float UNpcActivityInstanceBase::StopInteracting(const FNpcActorInteractionData* ActorInteractionData)
{
	float SuspendDelay = -1.f;
	if (!ActorInteractionData->IsInteracting() || ActorInteractionData->ActivityId != GetActivityId())
		return SuspendDelay;
	
	if (!ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.IsBoundToObject(this))
	{
		InteractionDelegateHandle = ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.AddUObject(this,
			&UNpcActivityInstanceBase::OnInteractionStateChanged);
	}

	auto Npc = Cast<IActivityCharacterProvider>(GetOuter())->GetActivityCharacter();;
	auto StopInteractionResult = INpcInteractableActor::Execute_StopNpcInteraction(ActorInteractionData->ActorToInteract, Npc, false);
	if (StopInteractionResult.bStopped)
		SuspendDelay = StopInteractionResult.Delay;

	return SuspendDelay;
}

void UNpcActivityInstanceBase::SetActorToIgnore(AActor* Actor)
{
	ClearNotIgnoredAnymoreActors();
	FActivityIgnoreActorData NewIgnoredActor;
	NewIgnoredActor.Actor = Actor;
	NewIgnoredActor.UntilWorldTime = GetOuter()->GetWorld()->GetTimeSeconds() + 60.f;
	IgnoredActors.Add(NewIgnoredActor);
}

void UNpcActivityInstanceBase::ResetInteractionActor()
{
	const auto ActorInteractionData = GetActorInteractionData();
	if (!ActorInteractionData->IsNone())
	{
		UnsubscribeFromInteractionAvailability();
		ClearActorInteractionData();
	}
}

void UNpcActivityInstanceBase::SetInteractionActor(UBlackboardComponent* Blackboard, AActor* ObservedActor, ENpcActorInteractionState InteractionState)
{
	const auto ActorInteractionData = GetActorInteractionData(); 
	if (!ActorInteractionData->IsNone())
		UnsubscribeFromInteractionAvailability();
	
	auto CurrentInteractionActor = Cast<INpcInteractableActor>(ObservedActor);
	if (!CurrentInteractionActor)
		return;
	
	SetActorInteractionData(ObservedActor, InteractionState);
	if (!CurrentInteractionActor->InteractionAvailableStateChangedEvent.IsBoundToObject(this))
	{
		InteractionActorAvailabilityDelegateHandler = CurrentInteractionActor->InteractionAvailableStateChangedEvent.AddUObject(this,
			&UNpcActivityInstanceBase::OnInteractionActorAvailableStateChanged);	
	}
	
	Blackboard->SetValueAsObject(BB_ActorToInteract, ObservedActor);
}

void UNpcActivityInstanceBase::PrepareForInteraction()
{
	const auto ActorInteractionData = GetActorInteractionData(); 
	if (!bActive || ActorInteractionData->IsNone())
		return;

	UnsubscribeFromInteractionAvailability();
	if (!ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.IsBoundToObject(this))
	{
		InteractionDelegateHandle = ActorInteractionData->GetInteractableActor()->InteractionStateChangedEvent.AddUObject(this,
			&UNpcActivityInstanceBase::OnInteractionStateChanged);
	}
}

void UNpcActivityInstanceBase::OnInteractionStateChanged(ANpcBaseInteractiveActor* InteractiveActor, ABaseCharacter* Interactor,
	ENpcActivityLatentActionState NpcActivityLatentActionState)
{
	if (!bActive)
		return;

	const auto ActorInteractionData = GetActorInteractionData(); 
	auto Character = Cast<IActivityCharacterProvider>(GetOuter())->GetActivityCharacter();
	if (Character != Interactor)
		return;
	
	if (!(ActorInteractionData->IsReleasing() || ActorInteractionData->IsInteracting()))
		return;

	InteractiveActor->InteractionStateChangedEvent.Remove(InteractionDelegateHandle);
	InteractionDelegateHandle.Reset();
	if (ActiveLatentActionCallback)
	{
		ActiveLatentActionCallback->ExecuteIfBound(NpcActivityLatentActionState);
		ActiveLatentActionCallback = nullptr;
	}
	
	// if (ActorInteractionData->IsReleasing())
	// 	ActorInteractionData->Reset();
	if (NpcActivityLatentActionState == ENpcActivityLatentActionState::Completed)
		ClearActorInteractionData();
	
	// ResetInteractionActor();
}

const FNpcActorInteractionData* UNpcActivityInstanceBase::GetActorInteractionData() const
{
	return NpcInteractionManager->GetCurrentNpcInteractionData();
}

void UNpcActivityInstanceBase::SetActorInteractionData(AActor* Actor, ENpcActorInteractionState State)
{
	UBlackboardComponent* Blackboard = Cast<IBlackboardProvider>(GetOuter())->GetBlackboard();
	NpcInteractionManager->SetCurrentNpcInteractionData(Actor, State, GetUniqueID());
	Blackboard->SetValueAsObject(BB_ActorToInteract, Actor); // ???
	Blackboard->SetValueAsEnum(BB_ActorInteractionState, (uint8)State);
}

void UNpcActivityInstanceBase::SetActorInteractionDataState(ENpcActorInteractionState State)
{
	UBlackboardComponent* Blackboard = Cast<IBlackboardProvider>(GetOuter())->GetBlackboard();
	NpcInteractionManager->SetCurrentNpcInteractionDataState(GetActivityId(), State);
	Blackboard->SetValueAsEnum(BB_ActorInteractionState, (uint8)State);
}

void UNpcActivityInstanceBase::ProlongInteractionMemory() const
{
	NpcInteractionManager->ProlongInteractionMemory();	
}

void UNpcActivityInstanceBase::ClearActorInteractionData() const
{
	NpcInteractionManager->ResetActorInteractionData();
	auto Blackboard = Cast<IBlackboardProvider>(GetOuter())->GetBlackboard();
	Blackboard->ClearValue(BB_ActorToInteract);
	Blackboard->SetValueAsEnum(BB_ActorInteractionState, (uint8)ENpcActorInteractionState::None);
}

void UNpcActivityInstanceBase::UnsubscribeFromInteractionAvailability()
{
	const auto ActorInteractionData = GetActorInteractionData(); 
	if (ActorInteractionData->GetInteractableActor()->InteractionAvailableStateChangedEvent.IsBoundToObject(this))
	{
		ActorInteractionData->GetInteractableActor()->InteractionAvailableStateChangedEvent.Remove(InteractionActorAvailabilityDelegateHandler);
		InteractionActorAvailabilityDelegateHandler.Reset();
	}
}

void UNpcActivityInstanceBase::OnInteractionActorAvailableStateChanged(AActor* InteractiveActor, bool bAvailable)
{
	if (!bActive)
		return;

	const auto ActorInteractionData = GetActorInteractionData(); 
	if (!bAvailable && ActorInteractionData->IsApproaching() && ActorInteractionData->ActorToInteract == InteractiveActor)
		ResetInteractionActor();
}

void UNpcActivityInstanceBase::ClearNotIgnoredAnymoreActors() const
{
	float WorldTime = GetOuter()->GetWorld()->GetTimeSeconds();
	IgnoredActors.RemoveAll([WorldTime](FActivityIgnoreActorData& Item) { return Item.UntilWorldTime < WorldTime; });
}

#pragma endregion INTERACTION

const AQuestLocation* UNpcActivityInstanceBase::GetClosestQuestLocation(const FDataTableRowHandle& LocationDTRH, const FVector& QuerierLocation) const
{
	auto WLS = GetOuter()->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WLS)
		return nullptr;

	return WLS->GetClosestQuestLocationComplex(LocationDTRH, QuerierLocation, GetOuter());
}

// just a draft
void UNpcActivityInstanceBase::Progress(float DeltaSeconds)
{
	if (ActivityProgressEvent.IsBound())
	{
		float EnergyChange = ActivitySettings->EnergyChange / ActivitySettings->EnergyChangeRate * DeltaSeconds;
		float BoredomChange = ActivitySettings->BoredomChange / ActivitySettings->BoredomChangeRate * DeltaSeconds;
		float UtilityChange = ActivitySettings->UtilityChange / ActivitySettings->UtilityChangeRate * DeltaSeconds;
		ActivityProgressEvent.Broadcast(EnergyChange, BoredomChange, UtilityChange);
	}
}

UEnvQuery* UNpcActivityInstanceBase::GetQueryTemplate(const FGameplayTag& QueryTag)
{
	auto Query = ActivitySettings->EnvQueries.Find(QueryTag);
	return Query ? *Query : nullptr;
}
