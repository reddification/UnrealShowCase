#include "NpcBaseInteractiveActor.h"

#include "AI/Interfaces/NpcCharacter.h"
#include "Characters/BaseCharacter.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Subsystems/QuestSubsystem.h"

ANpcBaseInteractiveActor::ANpcBaseInteractiveActor()
{
	StimulusSourceComponent = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimuli Source"));
	AddOwnedComponent(StimulusSourceComponent);

	StimulusSourceComponent->SetAutoActivate(true);
	StimulusSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
}

void ANpcBaseInteractiveActor::BeginPlay()
{
	Super::BeginPlay();
	StimulusSourceComponent->RegisterForSense(TSubclassOf<UAISense_Sight>());
	StimulusSourceComponent->RegisterWithPerceptionSystem();
	MaxInteractionsAtOnce = ActorInteractionOptions.Num();
	for (auto i = 0; i < MaxInteractionsAtOnce; i++)
		InteractingCharacters.Emplace(i, nullptr);
}

int ANpcBaseInteractiveActor::GetAvailableInteractionPosition(const FGameplayTag& InteractionTag) const
{
	for (auto i = 0; i < MaxInteractionsAtOnce; i++)
		if (!IsValid(InteractingCharacters[i].Character) && ActorInteractionOptions[i].InteractionTag == InteractionTag)
			return i;

	return -1;
}

int ANpcBaseInteractiveActor::GetInteractionPosition(const ABaseCharacter* Character)
{
	for (auto i = 0; i < MaxInteractionsAtOnce; i++)
		if (InteractingCharacters[i].Character == Character)
			return i;

	return -1;
}

void ANpcBaseInteractiveActor::RegisterNewInteraction(int InteractionOptionIndex, const FRunningInteractionData& NewInteraction)
{
	if (InteractionOptionIndex > MaxInteractionsAtOnce - 1)
		return;

	InteractingCharacters[InteractionOptionIndex] = NewInteraction;
	NewInteraction.Character->ChangeGameplayTags(ActorInteractionOptions[InteractionOptionIndex].InteractionTag.GetSingleTagContainer());
	NewInteraction.DeathDelegateHandle = NewInteraction.Character->CharacterDeathEvent.AddUObject(this, &ANpcBaseInteractiveActor::OnInteractionCharacterDied);
}

void ANpcBaseInteractiveActor::UnregisterInteraction(ABaseCharacter* Character)
{
	for (auto i = 0; i < MaxInteractionsAtOnce; i++)
	{
		if (InteractingCharacters[i].Character == Character)
		{
			ClearInteractionState(Character);
			Character->ChangeGameplayTags(ActorInteractionOptions[i].InteractionTag.GetSingleTagContainer(), false);
			Character->CharacterDeathEvent.Remove(InteractingCharacters[i].DeathDelegateHandle);
			InteractingCharacters[i].Reset();
			return;
		}
	}
}

void ANpcBaseInteractiveActor::BeginInteraction(int InteractionOptionIndex, const FRunningInteractionData& NewInteraction)
{
	RegisterNewInteraction(InteractionOptionIndex, NewInteraction);
	InteractionCounter++;
	if (InteractionAvailableStateChangedEvent.IsBound())
		InteractionAvailableStateChangedEvent.Broadcast(this, InteractionCounter < MaxInteractionsAtOnce);
	
	if (InteractionCounter < MaxInteractionsAtOnce)
		StimulusSourceComponent->RegisterWithPerceptionSystem();
	else StimulusSourceComponent->UnregisterFromPerceptionSystem();
}

void ANpcBaseInteractiveActor::FinishInteraction(ABaseCharacter* Character)
{
	UnregisterInteraction(Character);
	InteractionCounter--;
	if (InteractionAvailableStateChangedEvent.IsBound())
		InteractionAvailableStateChangedEvent.Broadcast(this, InteractionCounter < MaxInteractionsAtOnce);
	
	if (InteractionCounter < MaxInteractionsAtOnce)
		StimulusSourceComponent->RegisterWithPerceptionSystem();
	else StimulusSourceComponent->UnregisterFromPerceptionSystem();

	if (bQuestInteractionActor)
	{
		auto NpcCharacter = Cast<INpcCharacter>(Character);
		if (NpcCharacter)
		{
			auto QS = GetGameInstance()->GetSubsystem<UQuestSubsystem>();
			QS->OnActorInteracted(this, NpcCharacter->GetNpcDTRH());
		}
	}
}

void ANpcBaseInteractiveActor::ClearInteractionState(ABaseCharacter* Character)
{
}

FGameplayTag ANpcBaseInteractiveActor::GetValidInteractionTag(const FGameplayTag& InteractionTag) const
{
	if (!InteractionTag.IsValid())
		return DefaultInteractionTag;
	
	FGameplayTag ActualInteractionTag = DefaultInteractionTag;
	for (const auto& InteractionOption : ActorInteractionOptions)
	{
		if (InteractionOption.InteractionTag == InteractionTag)
		{
			ActualInteractionTag = InteractionTag;
			break;
		}
	}

	return ActualInteractionTag;
}

void ANpcBaseInteractiveActor::OnInteractionCharacterDied(const ABaseCharacter* InteractionCharacter)
{
	int Position = GetInteractionPosition(InteractionCharacter);
	if (Position >= 0)
		InteractingCharacters[Position].Reset();
}

bool ANpcBaseInteractiveActor::TryStartNpcInteraction_Implementation(ABaseCharacter* Character, const FGameplayTag& InteractionTag)
{
	FGameplayTag ActualInteractionTag = GetValidInteractionTag(InteractionTag);
	if (!INpcInteractableActor::Execute_IsNpcInteractionAvailable(this, ActualInteractionTag))
		return false;

	if (!IsValid(Character))
		return false;

	return true;
}

bool ANpcBaseInteractiveActor::StopNpcInteraction_Implementation(ABaseCharacter* Character, bool bInterupted)
{
	return true;
}

bool ANpcBaseInteractiveActor::GetInteractionLocation(const FGameplayTag& InteractionTag, FVector& OutLocation)
{
	for (auto i = 0; i < MaxInteractionsAtOnce; i++)
	{
		if (InteractingCharacters[i].IsActive())
			continue;

		if (ActorInteractionOptions[i].InteractionTag == InteractionTag)
		{
			OutLocation = GetActorLocation() + ActorInteractionOptions[i].InteractionPosition;
			return true;
		}
	}

	return false;
}

bool ANpcBaseInteractiveActor::IsNpcInteractionAvailable_Implementation(const FGameplayTag& InteractionTag) const
{
	if (InteractionCounter >= MaxInteractionsAtOnce)
		return false;

	FGameplayTag ActualInteractionTag = GetValidInteractionTag(InteractionTag);
	for (const auto& CharacterInteraction : InteractingCharacters)
		if (!CharacterInteraction.Value.IsActive() && ActorInteractionOptions[CharacterInteraction.Key].InteractionTag == ActualInteractionTag)
			return true;

	return false;
}

bool ANpcBaseInteractiveActor::GetNpcActiveInteractionTag(ABaseCharacter* Npc, FGameplayTag& OutTag) const
{
	for (const auto& ActorInteraction : InteractingCharacters)
	{
		if (ActorInteraction.Value.Character == Npc)
		{
			OutTag = ActorInteractionOptions[ActorInteraction.Key].InteractionTag;
			return true;
		}
	}

	return false;
}

void ANpcBaseInteractiveActor::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer = InteractableActorTags;
}

bool ANpcBaseInteractiveActor::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	return InteractableActorTags.HasTag(TagToCheck);
}

bool ANpcBaseInteractiveActor::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return InteractableActorTags.HasAll(TagContainer);
}

bool ANpcBaseInteractiveActor::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	return InteractableActorTags.HasAny(TagContainer);
}

void ANpcBaseInteractiveActor::ChangeGameplayTags(const FGameplayTagContainer& TagsContainer, bool bAdd)
{
	if (bAdd)
		InteractableActorTags.AppendTags(TagsContainer);
	else
		InteractableActorTags.RemoveTags(TagsContainer);
}
