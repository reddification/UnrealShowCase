#pragma once

#include "CoreMinimal.h"
#include "WorldStateSubsystem.h"
#include "Data/Quests/CutsceneData.h"
#include "Data/Quests/QuestDTR.h"
#include "Data/Quests/QuestProgress.h"
#include "Engine/DataTable.h"
#include "Game/MLCoreTypes.h"
#include "QuestSubsystem.generated.h"

class ULevelSequencePlayer;
struct FQuestSetSublevelStateAction;
struct FQuestPlayCutsceneAction;
struct FQuestCustomAction;
struct FQuestActions;
struct FQuestActionSpawnItems;
struct FQuestActionSpawnEnemies;

DECLARE_MULTICAST_DELEGATE_OneParam(FQuestStartedEvent, const FQuestDTR* QuestDTR)
DECLARE_MULTICAST_DELEGATE_OneParam(FQuestEventOccuredEvent, const FQuestTaskDTR* QuestTaskDTRH)
DECLARE_MULTICAST_DELEGATE_TwoParams(FQuestCompletedEvent, const FQuestDTR* QuestDTR, bool bAutocompleted)
DECLARE_MULTICAST_DELEGATE(FCutsceneStartedEvent);
DECLARE_MULTICAST_DELEGATE(FCutsceneEndedEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSublevelStateChangedEvent, TSoftObjectPtr<UWorld>, Sublevel, bool, bLoad);

UCLASS()
class ALUMCOCKSGJ_API UQuestSubsystem : public UGameInstanceSubsystem, public ISaveSubsystemInterface
{
	GENERATED_BODY()

public:
	void RegisterPlayerController(class ABasePlayerController* Controller);
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	void OnItemPickedUp(const UInventoryComponent* InventoryComponent,
		const FDataTableRowHandle& ItemDTRH, const FDataTableRowHandle& CharacterDTRH = FDataTableRowHandle());
	void OnLocationReached(const FDataTableRowHandle& LocationDTRH, const FDataTableRowHandle& CharacterDTRH = FDataTableRowHandle());
	void OnNpcKilled(const AActor* NPC);
	void OnActorInteracted(AActor* InteractiveQuestActor, const FDataTableRowHandle& CharacterDTRH = FDataTableRowHandle());
	// void OnNpcInteracted(const FDataTableRowHandle& NpcDTRH, const FGameplayTagContainer& NpcTags);
	void OnReadableRead(const FDataTableRowHandle& ItemDTRH);
	void AddQuest(const FDataTableRowHandle& QuestDTRH);
	void QuestEventOccured(const FDataTableRowHandle& QuestEventDtrh);

	void TrySkipCutscene() const;
	FJournalModel GetJournalData();
	FQuestNavigationGuidance GetNavigationGuidance(const FName& RowName);

	mutable FQuestStartedEvent QuestStartedEvent;
	mutable FQuestEventOccuredEvent QuestTaskCompletedEvent;
	mutable FQuestCompletedEvent QuestCompletedEvent;
	mutable FCutsceneStartedEvent CutsceneStartedEvent;
	mutable FCutsceneEndedEvent CutsceneEndedEvent;

	UPROPERTY(BlueprintAssignable)
	mutable FSublevelStateChangedEvent SublevelStateChangedEvent;
	
	bool IsItemRequirementsMatching(const FQuestRequirementItemFilter& ItemFilters) const;
	
private:
	
    UPROPERTY()
    const class APlayerCharacter* PlayerCharacter;

    UPROPERTY(SaveGame)
    TArray<FQuestProgress> ActiveQuests;

	UPROPERTY(SaveGame)
    TArray<FQuestProgress> CompletedQuests;
    
	bool CanStartQuest(const FDataTableRowHandle& QuestDTRH) const;
	bool CheckIfAlreadyHaveQuest(const TArray<FQuestProgress>& Quests, const FDataTableRowHandle& QuestDTRH) const;
   
	bool CheckIfComplyWithQuestRequirements(const FQuestDTR* QuestDTR) const;
	void GiveQuestTaskRewards(const FQuestTaskReward& QuestEventReward) const;
	void CompleteQuests(const TArray<FQuestProgress>& QuestsToComplete);
	void CompleteQuestTasks(FQuestProgress& QuestProgress, TArray<FQuestProgress>& QuestsToComplete,
		TArray<FQuestTaskInfo>& CompletedQuestTasks, bool bCovered = false);

	void ApplyWorldStateChanges(const FGameplayTagContainer& WorldStateChanges) const;
	void ExecuteQuestTaskActions(const FQuestActions& QuestActions);
	void TrySpawnEnemies(const TArray<FQuestActionSpawnEnemies>& SpawnEnemies, const FActorSpawnParameters& ActorSpawnParameters,
		const UWorldStateSubsystem* WSS) const;
	void SpawnEnemies(const FQuestActionSpawnEnemies& SpawnEnemiesData,
	                  const FActorSpawnParameters& ActorSpawnParameters,
	                  const FVector& PlayerLocation) const;
	void TrySpawnNpcs(const TArray<FQuestActionSpawnNpcs>& SpawnNpcs, const UWorldStateSubsystem* WSS) const;
	void TrySpawnItems(const TArray<FQuestActionSpawnItems>& SpawnItems, const FActorSpawnParameters& ActorSpawnParameters,
		const UWorldStateSubsystem* WSS) const;
	void SpawnItems(const FQuestActionSpawnItems& SpawnItemsAction, const FActorSpawnParameters& ActorSpawnParameters, const FVector& Vector) const;
	void TryExecuteCustomActions(const TArray<FQuestCustomAction>& CustomActions, const UWorldStateSubsystem* WorldStateSubsystem) const;
	void SetActiveCutsceneData(const FQuestPlayCutsceneAction& PlayCutsceneAction, ULevelSequencePlayer* ActiveCutscenePlayer);
	void TryPlayCutscene(const FQuestPlayCutsceneAction& PlayCutsceneAction, const UWorldStateSubsystem* WorldStateSubsystem);
	void TrySetLevelStates(const TArray<FQuestSetSublevelStateAction>& SublevelActions, const UWorldStateSubsystem* WorldStateSubsystem,
		bool bLoad) const;
	void TryDestroyAllActorsWithGameplayTags(const TArray<FDestroyActorsAction>& DestroyActorsAction) const;
	void TryRunNpcBehaviors(const TArray<FQuestActionNpcRunBehavior>& RunNpcBehaviorsActions, const UWorldStateSubsystem* WorldStateSubsystem) const;
	
	void InitializeQuest(const FDataTableRowHandle& QuestDTRH, const FQuestDTR* QuestDTR);
	bool IsQuestTaskPassItemFilter(const FQuestRequirementItemFilter& ItemsFilter) const;
	bool IsQuestTaskCompletable(const FQuestTaskInfo& QuestTaskInfo) const;
	bool HasRequiredGameplayTags(const IGameplayTagAssetInterface* GameplayTagActor, const FGameplayTagQuery& GameplayTagQuery) const;
	
	FVector GetRandomNavigableLocationNearPlayer(const FVector& PlayerLocation, float Radius, float FloorOffset) const;
	
	const AQuestLocation* GetQuestLocation(const FDataTableRowHandle& SpawnLocationDTRH) const;
	
	// TWeakObjectPtr<class UGCGameInstance> GameInstance;
	FActiveCusceneData ActiveCutsceneData;
	
	UFUNCTION()
	void OnCutsceneFinished();

	void HandleCoveredTasks();
};
