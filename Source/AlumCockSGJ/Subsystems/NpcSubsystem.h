#pragma once

#include "CoreMinimal.h"
#include "CommonConstants.h"
#include "AI/Characters/AICitizenCharacter.h"
#include "AI/Data/NpcReactionTypes.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Data/Quests/QuestAction.h"
#include "Engine/DataTable.h"
#include "NpcSubsystem.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UNpcSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	void RegisterNpc(const FDataTableRowHandle& DTRH, ABaseCharacter* Npc);
	void RegisterPlayerController(const ABasePlayerController* NewPlayerController);
	FNpcReactResult TryReact(ABaseCharacter* Npc, const FGameplayTag& ReactionTag);
	bool TryFinishReaction(ABaseCharacter* Npc, const FGameplayTag& ReactionType,
		int ReactionIndex, const FName& MontageEndLoopSectionName = MontageSectionEndLoop);
	bool TryRunQuestBehavior(const FQuestActionNpcRunBehavior& RunBehaviorData);
	bool RunQuestBehavior(ABaseCharacter* Npc, const FNpcQuestBehaviorDescriptor& BehaviorDescriptor) const;
	ABaseCharacter* GetNpc(const FDataTableRowHandle& NpcDTRH);
	ABaseCharacter* SpawnNpc(const FQuestActionSpawnNpcs& SpawnNpcAction, const FVector& SpawnLocation);

	FRotator GetFacePlayerViewpointRotation(const FVector& ObserverLocation) const;

private:
	TMultiMap<FName, ABaseCharacter*> Npcs;
	ABaseCharacter* FindNpc(const FDataTableRowHandle& NpcDTRH, bool bSpawnNew);

	UPROPERTY()
	const ABasePlayerController* PlayerController;

	void OnNpcDied(const FDataTableRowHandle& NpcDTRH, ABaseCharacter* NpcCharacter);
};
