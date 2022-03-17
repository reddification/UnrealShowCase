#include "NpcQuestActivityInstance.h"

#include "AI/Interfaces/ActivityCharacterProvider.h"
#include "Characters/BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/WorldStateSubsystem.h"

void UNpcQuestActivityInstance::Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor,
	AAIController* AIController)
{
	Super::Setup(NpcBehaviorDescriptor, AIController);
	QuestSettings = Cast<UNpcQuestActivityBaseSettings>(NpcBehaviorDescriptor.NpcActivitySettings);
	RetriesLeft = QuestSettings->RetryLimit;
	if (QuestSettings->TimeLimit > 0.f)
	{
		AIController->GetWorld()->GetTimerManager().SetTimer(Timer, this,
			&UNpcQuestActivityInstance::ActivityTimerElapsed, QuestSettings->TimeLimit);
		AIController->GetWorld()->GetTimerManager().PauseTimer(Timer);
	}
}

void UNpcQuestActivityInstance::Resume()
{
	Super::Resume();
	if (bStateAchieved)
		ReportActivityEvent(EActivityEventType::Completed);

	if (QuestSettings->TimeLimit > 0.f)
		GetOuter()->GetWorld()->GetTimerManager().UnPauseTimer(Timer);
}

float UNpcQuestActivityInstance::Suspend(AAIController* AIController, bool bAbortInteraction)
{
	if (AIController->GetWorld()->GetTimerManager().IsTimerActive(Timer))
		AIController->GetWorld()->GetTimerManager().PauseTimer(Timer);

	return Super::Suspend(AIController, bAbortInteraction);
}

void UNpcQuestActivityInstance::OnWorldStateChanged(const FGameplayTagContainer& NewWorldState)
{
	if (UntilWorldState.Matches(NewWorldState))
		FinishQuestActivity();
}

void UNpcQuestActivityInstance::OnCharacterStateChanged(const FGameplayTagContainer& NewCharacterState)
{
	if (UntilCharacterState.Matches(NewCharacterState))
		FinishQuestActivity();
}

void UNpcQuestActivityInstance::ActivityTimerElapsed()
{
	EActivityEventType Result = QuestSettings->bQuestActivitySuccessOnTimerEnd
		? EActivityEventType::Completed
		: EActivityEventType::Timeout;
	ReportActivityEvent(Result);
}

void UNpcQuestActivityInstance::FinishQuestActivity(EActivityEventType Result)
{
	const auto ActorInteractionData = GetActorInteractionData();
	if (ActorInteractionData->IsInteracting() && ActorInteractionData->ActivityId == GetActivityId())
		StopInteracting(ActorInteractionData);

	bStateAchieved = true;
	if (CharacterStateChangedEventHandle.IsValid())
	{
		auto BaseCharacter = Cast<IActivityCharacterProvider>(GetOuter())->GetActivityCharacter();
		BaseCharacter->CharacterGameplayStateChangedEvent.Remove(CharacterStateChangedEventHandle);
		CharacterStateChangedEventHandle.Reset();
	}

	if (WorldStateChangedEventHandle.IsValid())
	{
		auto WSS = UGameplayStatics::GetGameInstance(GetOuter())->GetSubsystem<UWorldStateSubsystem>();
		WSS->WorldStateChangedEvent.Remove(WorldStateChangedEventHandle);
		WorldStateChangedEventHandle.Reset();
	}

	// must be last
	NpcActivityEvent.ExecuteIfBound(Result);
}

void UNpcQuestActivityInstance::SetStopConditions(const AAIController* AIController, const FNpcQuestBehaviorDescriptor& QuestBehaviorDescriptor)
{
	UntilWorldState = QuestBehaviorDescriptor.UntilWorldState;
	UntilCharacterState = QuestBehaviorDescriptor.UntilCharacterState;
	
	auto WSS = AIController->GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	if (!UntilWorldState.IsEmpty())
		WorldStateChangedEventHandle = WSS->WorldStateChangedEvent.AddUObject(this, &UNpcQuestActivityInstance::OnWorldStateChanged);
	
	auto BaseCharacter = Cast<ABaseCharacter>(AIController->GetPawn());
	if (!UntilCharacterState.IsEmpty())
		CharacterStateChangedEventHandle = BaseCharacter->CharacterGameplayStateChangedEvent.AddUObject(this, &UNpcQuestActivityInstance::OnCharacterStateChanged);
}

void UNpcQuestActivityInstance::ReportActivityEvent(EActivityEventType UtilityChange)
{
	switch (UtilityChange)
	{
		case EActivityEventType::Assessment:
		case EActivityEventType::Iteration:
		case EActivityEventType::ArbitraryFailure:
		case EActivityEventType::Inoperable:
			if (RetriesLeft == 1)
			{
				EActivityEventType Result = QuestSettings->bQuestActivitySuccessOnTimerEnd
					? EActivityEventType::Completed
					: EActivityEventType::Timeout;
				
				FinishQuestActivity(Result);
			}
			else
			{
				RetriesLeft--;
			}
		break;
		case EActivityEventType::Completed:
			FinishQuestActivity();
		default:
		break;
	}
}
