#include "NpcPlayerInteractionComponent.h"

#include "AI/Data/NpcDTR.h"
#include "AI/Data/NpcPlayerInteractionDTR.h"
#include "AI/Interfaces/NpcActivityManager.h"
#include "AI/Interfaces/NpcCharacter.h"
#include "Subsystems/DialogueSubsystem.h"
#include "Subsystems/NpcSubsystem.h"
#include "Subsystems/QuestSubsystem.h"
#include "Subsystems/WorldStateSubsystem.h"

void UNpcPlayerInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	NpcCharacter = GetOwner();
}

bool UNpcPlayerInteractionComponent::StartInteractWithPlayer(ABaseCharacter* Interactor)
{
	if (bInteracting)
		return false;
	
	FGameplayTagContainer CurrentCharacterState;
	OwnerCharacter->GetOwnedGameplayTags(CurrentCharacterState);
	
	if (!UnableToInteractInStates.IsEmpty() && UnableToInteractInStates.Matches(CurrentCharacterState))
		return false;
	
	FNpcDTR* NpcDTR = NpcCharacter->GetNpcDTRH().GetRow<FNpcDTR>("");
	if (!NpcDTR)
		return false;
	
	if (NpcDTR->NpcPlayerInteractions)
	{
		auto WSS = GetWorld()->GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
		auto QuestSystem = GetWorld()->GetGameInstance()->GetSubsystem<UQuestSubsystem>();
		auto DialogueSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<UDialogueSubsystem>();

		const FGameplayTagContainer& CurrentWorldState = WSS->GetWorldState();
		const auto RawRows = NpcDTR->NpcPlayerInteractions->GetRowMap();

		const FNpcPlayerInteractionDTR* InteractionData = nullptr;
		TArray<const FNpcPlayerInteractionDTR*> ValidInteractions;
		FGameplayTagContainer PlayerGameplayTags;
		Interactor->GetOwnedGameplayTags(PlayerGameplayTags);
		for (const auto& RawRow : RawRows)
		{
			InteractionData = reinterpret_cast<FNpcPlayerInteractionDTR*>(RawRow.Value);
			if (	!WSS->IsWorldStateMatches(InteractionData->PlayerRequirements.RequiresWorldState)
				||  !QuestSystem->IsItemRequirementsMatching(InteractionData->PlayerRequirements.ItemFilters)
				||  (!InteractionData->NpcGameplayStateRequired.IsEmpty() && !InteractionData->NpcGameplayStateRequired.Matches(CurrentCharacterState))
				||  (!InteractionData->PlayerRequirements.RequiresPlayerGameplayState.IsEmpty() && !InteractionData->PlayerRequirements.RequiresPlayerGameplayState.Matches(PlayerGameplayTags)))
			{
				continue;
			}

			ValidInteractions.Emplace(InteractionData);
		}

		if (ValidInteractions.Num() == 1)
		{
			bInteracting = DialogueSubsystem->StartDialogue(ValidInteractions[0], OwnerCharacter, NpcDTR);
		}
		else
		{
			// TODO show dialogue box with options
			if (ValidInteractions.Num() > 0)
				bInteracting = DialogueSubsystem->StartDialogue(ValidInteractions[0], OwnerCharacter, NpcDTR);
		}
	}

	return bInteracting;
}