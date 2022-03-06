#include "QuestSubsystem.h"

#include "EngineUtils.h"
#include "LevelSequenceActor.h"
#include "NavigationSystem.h"
#include "NpcSubsystem.h"
#include "WorldLocationsSubsystem.h"
#include "WorldStateSubsystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Controllers/AIBaseController.h"
#include "Characters/PlayerCharacter.h"
#include "Components/Character/InventoryComponent.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Kismet/GameplayStatics.h"

void UQuestSubsystem::RegisterPlayerController(ABasePlayerController* Controller)
{
	PlayerCharacter = Cast<APlayerCharacter>(Controller->GetControlledCharacter());
}

void UQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	ActiveQuests.Empty();
	CompletedQuests.Empty();
}

#pragma region QUEST TASKS

void UQuestSubsystem::OnItemPickedUp(const UInventoryComponent* InventoryComponent,
	const FDataTableRowHandle& ItemDTRH, const FDataTableRowHandle& CharacterDTRH)
{
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& QuestProgress : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CompletedQuestTasks;
		for (const auto& QuestTaskInfo : QuestProgress.PendingQuestTasks)
		{
			if (!IsQuestTaskCompletable(QuestTaskInfo))
				continue;

			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			const auto& ItemsAcquiredTrigger = QuestTaskDTR->TaskCompletedTriggers.ItemsAcquired;
			if (ItemsAcquiredTrigger.WorldItemDTRH == ItemDTRH && (ItemsAcquiredTrigger.ByCharacterDTRH.IsNull() || ItemsAcquiredTrigger.ByCharacterDTRH == CharacterDTRH))
			{
				if (InventoryComponent->GetCountOfItem(ItemDTRH) == ItemsAcquiredTrigger.Count)
					CompletedQuestTasks.Add(QuestTaskInfo);
			}
		}

		CompleteQuestTasks(QuestProgress, QuestsToComplete, CompletedQuestTasks);
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::OnLocationReached(const FDataTableRowHandle& LocationDTRH, const FDataTableRowHandle& CharacterDTRH)
{
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& QuestProgress : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CompletedQuestEvents;
		for (const auto& QuestTaskInfo : QuestProgress.PendingQuestTasks)
		{
			if (!IsQuestTaskCompletable(QuestTaskInfo))
				continue;

			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			const auto& LocationReachedTrigger = QuestTaskDTR->TaskCompletedTriggers.LocationsReached;
			if (LocationReachedTrigger.LocationDTRH == LocationDTRH && (LocationReachedTrigger.ByCharacterDTRH.IsNull() || LocationReachedTrigger.ByCharacterDTRH == CharacterDTRH))
			{
				bool bCanCompleteQuestTask = LocationReachedTrigger.ItemPossessionFilter.bActive
					? IsQuestTaskPassItemFilter(LocationReachedTrigger.ItemPossessionFilter)
					: true;
				
				if (bCanCompleteQuestTask)
					CompletedQuestEvents.Add(QuestTaskInfo);
			}
		}

		CompleteQuestTasks(QuestProgress, QuestsToComplete, CompletedQuestEvents);
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::OnNpcKilled(const AActor* NPC)
{
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& QuestProgress : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CompletedQuestTasks;
		for (auto& QuestTaskInfo : QuestProgress.PendingQuestTasks)
		{
			if (!IsQuestTaskCompletable(QuestTaskInfo))
				continue;

			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			const auto& NPCsKilledTrigger = QuestTaskDTR->TaskCompletedTriggers.NPCsKilled;
			if (NPC->IsA(NPCsKilledTrigger.ActorType) && HasRequiredGameplayTags(Cast<IGameplayTagAssetInterface>(NPC), NPCsKilledTrigger.WithGameplayTags))
			{
				QuestTaskInfo.CountProgress++;
				if (QuestTaskInfo.CountProgress == NPCsKilledTrigger.Count)
					CompletedQuestTasks.Add(QuestTaskInfo);
			}
		}

		CompleteQuestTasks(QuestProgress, QuestsToComplete, CompletedQuestTasks);
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::OnActorInteracted(AActor* InteractiveQuestActor, const FDataTableRowHandle& CharacterDTRH)
{
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& QuestProgress : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CompletedQuestTasks;
		for (auto& QuestTaskInfo : QuestProgress.PendingQuestTasks)
		{
			if (!IsQuestTaskCompletable(QuestTaskInfo))
				continue;

			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			const auto& InteractWithActorTrigger = QuestTaskDTR->TaskCompletedTriggers.ActorsInteracted;
			bool bCanCompleteQuestTask = InteractiveQuestActor->IsA(InteractWithActorTrigger.ActorType)
				&& (InteractWithActorTrigger.ByCharacter.IsNull() || InteractWithActorTrigger.ByCharacter == CharacterDTRH)
				&& HasRequiredGameplayTags(Cast<const IGameplayTagAssetInterface>(InteractiveQuestActor), InteractWithActorTrigger.WithGameplayTags)
				&& (!InteractWithActorTrigger.ItemPossessionFilter.bActive || IsQuestTaskPassItemFilter(InteractWithActorTrigger.ItemPossessionFilter));
			
			if (bCanCompleteQuestTask)
				CompletedQuestTasks.Add(QuestTaskInfo);
		}

		CompleteQuestTasks(QuestProgress, QuestsToComplete, CompletedQuestTasks);
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::OnReadableRead(const FDataTableRowHandle& ItemDTRH)
{
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& QuestProgress : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CompletedQuestTasks;
		for (auto& QuestTaskInfo : QuestProgress.PendingQuestTasks)
		{
			if (!IsQuestTaskCompletable(QuestTaskInfo))
				continue;

			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			const auto& InteractWithActorTrigger = QuestTaskDTR->TaskCompletedTriggers.ReadableRead;
			if (InteractWithActorTrigger == ItemDTRH)
			{
				CompletedQuestTasks.Add(QuestTaskInfo);
				break; // why would there be 2 quest tasks in the same quest that require player to read something?
			}
		}

		CompleteQuestTasks(QuestProgress, QuestsToComplete, CompletedQuestTasks);
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::QuestEventOccured(const FDataTableRowHandle& QuestEventDtrh)
{
	// wtf is this situation
	if (ActiveQuests.Num() <= 0)
		return;
	
	TArray<FQuestProgress> QuestsToComplete;
	TArray<FQuestTaskInfo> CompletedQuestTasks;
	
	bool bQuestFound = false;
	for (auto& ActiveQuest : ActiveQuests)
	{
		for (auto& PendingQuestTask : ActiveQuest.PendingQuestTasks)
		{
			if (PendingQuestTask.QuestTaskDTRH == QuestEventDtrh)
			{
				if (IsQuestTaskCompletable(PendingQuestTask))
				{
					CompletedQuestTasks.Add(PendingQuestTask);
					CompleteQuestTasks(ActiveQuest, QuestsToComplete, CompletedQuestTasks);	
				}

				bQuestFound = true;
				break;
			}
		}

		if (bQuestFound)
			break;
	}

	CompleteQuests(QuestsToComplete);
	HandleCoveredTasks();
}

void UQuestSubsystem::CompleteQuests(const TArray<FQuestProgress>& QuestsToComplete)
{
	for (const auto& CompletedQuest : QuestsToComplete)
	{
		CompletedQuests.Add(CompletedQuest);
		ActiveQuests.Remove(CompletedQuest);

		bool bQuestAutocompleted = CompletedQuest.CompletedQuestTasks.Last().GetQuestTaskDTR()->QuestState == EQuestState::Completed;
		FQuestDTR* QuestDTR = CompletedQuest.QuestDTRH.DataTable->FindRow<FQuestDTR>(CompletedQuest.QuestDTRH.RowName, "");
		if (QuestDTR)
		{
			ExecuteQuestTaskActions(QuestDTR->EndQuestActions);
		
			if (QuestCompletedEvent.IsBound())
				QuestCompletedEvent.Broadcast(QuestDTR, bQuestAutocompleted);
		}
	}
}

void UQuestSubsystem::CompleteQuestTasks(FQuestProgress& QuestProgress, TArray<FQuestProgress>& QuestsToComplete,
	TArray<FQuestTaskInfo>& CompletedQuestTasks, bool bCovered)
{
	for (const auto& CompletedQuestTaskInfo : CompletedQuestTasks)
	{
		if (bCovered)
			QuestProgress.CoveredQuestsTasks.Add(CompletedQuestTaskInfo);
		else 
			QuestProgress.CompletedQuestTasks.Add(CompletedQuestTaskInfo);
		
		QuestProgress.PendingQuestTasks.Remove(CompletedQuestTaskInfo);
		
		auto CompletedTaskDTR = CompletedQuestTaskInfo.GetQuestTaskDTR();
		if (!bCovered || CompletedTaskDTR->bExecuteActionsWhenCovered)
		{
			ExecuteQuestTaskActions(CompletedTaskDTR->TaskOccuredActions);
			GiveQuestTaskRewards(CompletedTaskDTR->QuestEventReward);
		}
		
		if (QuestTaskCompletedEvent.IsBound())
			QuestTaskCompletedEvent.Broadcast(CompletedTaskDTR);
		
		bool bNoPendingTasksLeft = true;
		for (const auto& PendingQuestTask : QuestProgress.PendingQuestTasks)
		{
			FQuestTaskDTR* QuestTaskDTR = PendingQuestTask.GetQuestTaskDTR();
			if (!QuestTaskDTR->bImplicit)
			{
				bNoPendingTasksLeft = false;
				break;
			}
		}
		
		if (bNoPendingTasksLeft || CompletedTaskDTR->QuestState == EQuestState::Completed)
			QuestsToComplete.AddUnique(QuestProgress);
	}
}

bool UQuestSubsystem::HasRequiredGameplayTags(const IGameplayTagAssetInterface* GameplayTagActor,
	const FGameplayTagQuery& GameplayTagQuery) const
{
	if (!GameplayTagActor || GameplayTagQuery.IsEmpty())
		return true;
	
	FGameplayTagContainer ActorsTags;
	GameplayTagActor->GetOwnedGameplayTags(ActorsTags);
	return GameplayTagQuery.Matches(ActorsTags);
}

bool UQuestSubsystem::IsQuestTaskCompletable(const FQuestTaskInfo& QuestTaskInfo) const
{
	auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
	if (!QuestTaskDTR->bActive)
		return false;

	auto WorldStateSubsystem = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	return (QuestTaskDTR->CoveredByWorldState.IsEmpty() || !WorldStateSubsystem->IsWorldStateMatches(QuestTaskDTR->CoveredByWorldState))
		&& (WorldStateSubsystem->IsWorldStateMatches(QuestTaskDTR->QuestTaskRequirements.RequiresWorldState));
}

#pragma endregion QUEST TASKS

#pragma region START QUEST

void UQuestSubsystem::AddQuest(const FDataTableRowHandle& QuestDTRH)
{
	if (!CanStartQuest(QuestDTRH))
		return;
	
	FQuestDTR* QuestDTR = QuestDTRH.DataTable->FindRow<FQuestDTR>(QuestDTRH.RowName, "");
	InitializeQuest(QuestDTRH, QuestDTR);
	ExecuteQuestTaskActions(QuestDTR->BeginQuestActions);
	
	if (QuestStartedEvent.IsBound())
		QuestStartedEvent.Broadcast(QuestDTR);
}

void UQuestSubsystem::InitializeQuest(const FDataTableRowHandle& QuestDTRH, const FQuestDTR* QuestDTR)
{
	FQuestProgress QuestProgress;
	QuestProgress.QuestDTRH = QuestDTRH;
	auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	TArray<FName> QuestEventsDataTableRowNames = QuestDTR->QuestEventsDT->GetRowNames();
	for (const auto& RowName : QuestEventsDataTableRowNames)
	{
		FDataTableRowHandle DTRH;
		DTRH.DataTable = QuestDTR->QuestEventsDT;
		DTRH.RowName = RowName;
		FQuestTaskDTR* QuestTaskDTR = DTRH.GetRow<FQuestTaskDTR>("");
		FQuestTaskInfo QuestEventInfo;
		QuestEventInfo.QuestTaskDTRH = DTRH;
		if (QuestTaskDTR->CoveredByWorldState.IsEmpty() || !WSS->IsWorldStateMatches(QuestTaskDTR->CoveredByWorldState))
			QuestProgress.PendingQuestTasks.Add(QuestEventInfo);
		else
			QuestProgress.CoveredQuestsTasks.Add(QuestEventInfo);
	}
	
	ActiveQuests.Add(QuestProgress);
}

bool UQuestSubsystem::CanStartQuest(const FDataTableRowHandle& QuestDTRH) const
{
	if (CheckIfAlreadyHaveQuest(ActiveQuests, QuestDTRH) || CheckIfAlreadyHaveQuest(CompletedQuests, QuestDTRH))
		return false;

	FQuestDTR* QuestDTR = QuestDTRH.DataTable->FindRow<FQuestDTR>(QuestDTRH.RowName, "");
	if (QuestDTR == nullptr)
		return false;

	if (!IsItemRequirementsMatching(QuestDTR->QuestRequirements.ItemFilters) || !CheckIfComplyWithQuestRequirements(QuestDTR))
		return false;

	return true;
}

bool UQuestSubsystem::IsItemRequirementsMatching(const FQuestRequirementItemFilter& ItemFilters) const
{
	if (!ItemFilters.bActive || ItemFilters.MustPossessItems.Num() <= 0)
		return true;
	
	const auto InventoryComponent = PlayerCharacter->GetInventoryComponent();
	switch (ItemFilters.ContainmentType)
	{
		case EContainmentType::Any:
			for (const auto& QuestItemFilter : ItemFilters.MustPossessItems)
			{
				if (InventoryComponent->HasItem(QuestItemFilter) == ItemFilters.bMustPossess)
					return true;
			}
		
		return false;
		case EContainmentType::All:
			for (const auto& QuestItemFilter : ItemFilters.MustPossessItems)
			{
				if (InventoryComponent->HasItem(QuestItemFilter) != ItemFilters.bMustPossess)
					return false;
			}
		
		return true;
		default:
			return true;
	}
}

bool UQuestSubsystem::CheckIfComplyWithQuestRequirements(const FQuestDTR* QuestDTR) const
{
	if (QuestDTR->QuestRequirements.RequiresWorldState.IsEmpty())
		return true;
	
	auto WorldStateSubsystem = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	return WorldStateSubsystem->IsWorldStateMatches(QuestDTR->QuestRequirements.RequiresWorldState);
}

void UQuestSubsystem::GiveQuestTaskRewards(const FQuestTaskReward& QuestEventReward) const
{
	
}

bool UQuestSubsystem::CheckIfAlreadyHaveQuest(const TArray<FQuestProgress>& Quests, const FDataTableRowHandle& QuestDTRH) const
{
	for (const auto QuestProgress: Quests)
	{
		if (QuestProgress.QuestDTRH == QuestDTRH)
			return true;
	}
	
	return false;
}

#pragma endregion START QUEST

#pragma region QUEST ACTIONS

void UQuestSubsystem::ExecuteQuestTaskActions(const FQuestActions& QuestActions)
{
	FActorSpawnParameters ActorSpawnParameters;
	ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	TrySetLevelStates(QuestActions.SetSublevelsStates, WSS, true);
	ApplyWorldStateChanges(QuestActions.WorldStateChanges);
	TrySpawnEnemies(QuestActions.SpawnEnemies, ActorSpawnParameters, WSS);
	TrySpawnNpcs(QuestActions.SpawnNpcs, WSS);
	TrySpawnItems(QuestActions.SpawnItems, ActorSpawnParameters, WSS);
	TryPlayCutscene(QuestActions.PlayCutscene, WSS);
	TryExecuteCustomActions(QuestActions.CustomActions, WSS);
	TryDestroyAllActorsWithGameplayTags(QuestActions.DestroyActors);
	TryRunNpcBehaviors(QuestActions.RunNpcBehaviors, WSS);					
	TrySetLevelStates(QuestActions.SetSublevelsStates, WSS, false);
}

void UQuestSubsystem::ApplyWorldStateChanges(const FGameplayTagContainer& WorldStateChanges) const
{
	auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	WSS->ChangeWorldState(WorldStateChanges);
}

void UQuestSubsystem::TrySpawnEnemies(const TArray<FQuestActionSpawnEnemies>& SpawnEnemiesActions,
	const FActorSpawnParameters& ActorSpawnParameters, const UWorldStateSubsystem* WSS) const
{
	if (SpawnEnemiesActions.Num() <= 0)
		return;
	
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	for (const auto SpawnEnemiesAction : SpawnEnemiesActions)
		if (WSS->IsWorldStateMatches(SpawnEnemiesAction.OnlyAtWorldState))
			SpawnEnemies(SpawnEnemiesAction, ActorSpawnParameters, PlayerLocation);
}

void UQuestSubsystem::SpawnEnemies(const FQuestActionSpawnEnemies& SpawnEnemiesData, const FActorSpawnParameters& ActorSpawnParameters,
	const FVector& PlayerLocation) const
{
	const AQuestLocation* QuestLocation = GetQuestLocation(SpawnEnemiesData.SpawnLocationDTRH);
	if (!IsValid(QuestLocation))
		return;
	
	for (auto i = 0; i < SpawnEnemiesData.Count; i++)
	{
		FVector SpawnLocation = QuestLocation->GetRandomLocationInVolume(SpawnEnemiesData.FloorOffset);
		APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(SpawnEnemiesData.EnemyClass, SpawnLocation,
			FRotator::ZeroRotator, ActorSpawnParameters);
		if (!SpawnEnemiesData.NpcDTRH.IsNull())
		{
			auto DatatableInitializablePawn = Cast<IDatatableInitializablePawn>(SpawnedPawn);
			if (DatatableInitializablePawn)
				DatatableInitializablePawn->SetNpcDTRH(SpawnEnemiesData.NpcDTRH);
		}
		
		if (!SpawnEnemiesData.WithTag_Old.IsNone())
			SpawnedPawn->Tags.Add(SpawnEnemiesData.WithTag_Old);

		if (!SpawnEnemiesData.WithTags.IsEmpty())
		{
			auto GameplayAsset = Cast<IGameplayTagActor>(SpawnedPawn);
			if (GameplayAsset)
				GameplayAsset->ChangeGameplayTags(SpawnEnemiesData.WithTags);
		}
		
		SpawnedPawn->SpawnDefaultController();
		if (SpawnEnemiesData.InitialInterest != ENpcInitialInterest::None)
		{
			auto AIController = Cast<AAIBaseController>(SpawnedPawn->Controller);
			if (IsValid(AIController))
			{
				switch (SpawnEnemiesData.InitialInterest)
				{
				case ENpcInitialInterest::Player:
					AIController->SetInterestingLocation(PlayerLocation);
					break;
				case ENpcInitialInterest::Location:
					{
						auto TargetLocation = GetQuestLocation(SpawnEnemiesData.InterestingLocationDTRH);
						if (IsValid(TargetLocation))
							AIController->SetInterestingLocation(TargetLocation->GetActorLocation());
					}
					break;
				default:
					break;
				}
			}	
		}
	}
}

void UQuestSubsystem::TrySpawnNpcs(const TArray<FQuestActionSpawnNpcs>& SpawnNpcs, const UWorldStateSubsystem* WSS) const
{
	if (SpawnNpcs.Num() <= 0)
		return;

	auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
	auto WorldLocationSubsystem = GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	for (const auto& SpawnNpcAction : SpawnNpcs)
	{
		if (!WSS->IsWorldStateMatches(SpawnNpcAction.OnlyAtWorldState))
			continue;
		
		for (auto i = 0; i < SpawnNpcAction.Count; i++)
		{
			auto WorldLocation = WorldLocationSubsystem->GetClosestQuestLocationSimple(SpawnNpcAction.SpawnLocationDTRH,
				PlayerCharacter->GetActorLocation());

			if (!WorldLocation)
				continue;
			
			ABaseCharacter* Npc = NpcSubsystem->SpawnNpc(SpawnNpcAction, WorldLocation->GetRandomLocationInVolume(100.f));
			if (!Npc)
				continue;

			if (SpawnNpcAction.NpcQuestBehaviorDescriptor.BehaviorDescriptor.BehaviorTree)
				NpcSubsystem->RunQuestBehavior(Npc, SpawnNpcAction.NpcQuestBehaviorDescriptor);
		}	
	}
}

void UQuestSubsystem::TrySpawnItems(const TArray<FQuestActionSpawnItems>& SpawnItemsActions,
	const FActorSpawnParameters& ActorSpawnParameters, const UWorldStateSubsystem* WSS) const
{
	if (SpawnItemsActions.Num() <= 0)
		return;
	
	FVector PlayerLocation = PlayerCharacter->GetActorLocation();
	for (const auto& SpawnItemsAction : SpawnItemsActions)
		if (WSS->IsWorldStateMatches(SpawnItemsAction.OnlyAtWorldState))
			SpawnItems(SpawnItemsAction, ActorSpawnParameters, PlayerLocation);
}

void UQuestSubsystem::SpawnItems(const FQuestActionSpawnItems& SpawnItemsAction,
	const FActorSpawnParameters& ActorSpawnParameters, const FVector& PlayerLocation) const
{
	const AQuestLocation* TargetQuestLocation = GetQuestLocation(SpawnItemsAction.SpawnLocationDTRH);
	if (!IsValid(TargetQuestLocation))
		return;

	FWorldItemDTR* WorldItemDTR = SpawnItemsAction.WorldItemDTRH.DataTable->FindRow<FWorldItemDTR>(SpawnItemsAction.WorldItemDTRH.RowName, "");
	if (!WorldItemDTR)
		return;
		
	for (auto i = 0; i < SpawnItemsAction.Count; i++)
	{
		FVector SpawnLocation = SpawnItemsAction.bNearPlayer
			? GetRandomNavigableLocationNearPlayer(PlayerLocation, SpawnItemsAction.NearPlayerRadius, SpawnItemsAction.FloorOffset)
			: TargetQuestLocation->GetRandomLocationInVolume(SpawnItemsAction.FloorOffset);
		if (!IsValid(WorldItemDTR->SpawnWorldItemClass))
			continue;
			
		ABaseWorldItem* SpawnedPickable = GetWorld()->SpawnActor<ABaseWorldItem>(WorldItemDTR->SpawnWorldItemClass,
			SpawnLocation, FRotator::ZeroRotator, ActorSpawnParameters);
		SpawnedPickable->SetItemDTRH(SpawnItemsAction.WorldItemDTRH);
		if (!SpawnItemsAction.WithTag_Old.IsNone())
			SpawnedPickable->Tags.Add(SpawnItemsAction.WithTag_Old);
		
		if (!SpawnItemsAction.WithTags.IsEmpty())
			SpawnedPickable->ChangeGameplayTags(SpawnItemsAction.WithTags, true);	
	}
}

FVector UQuestSubsystem::GetRandomNavigableLocationNearPlayer(const FVector& PlayerLocation, float Radius, float FloorOffset) const
{
	FVector OutLocation;
	bool bLocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), PlayerLocation,
		OutLocation, Radius);

	return (bLocationFound ? OutLocation : PlayerLocation) + FVector::UpVector * FloorOffset;
}

void UQuestSubsystem::TryExecuteCustomActions(const TArray<FQuestCustomAction>& CustomActions, const UWorldStateSubsystem* WSS) const
{
	if (CustomActions.Num() <= 0)
		return;
	
	for (const auto& CustomAction : CustomActions)
	{
		if (WSS->IsWorldStateMatches(CustomAction.OnlyAtWorldState))
		{
			auto CustomActionActor = GetWorld()->SpawnActor<AQuestCustomActionBase>(CustomAction.CustomActionClass);
			CustomActionActor->ExecuteCustomAction();
		}
	}
}

void UQuestSubsystem::TryPlayCutscene(const FQuestPlayCutsceneAction& PlayCutsceneAction, const UWorldStateSubsystem* WSS)
{
	if (!PlayCutsceneAction.Cutscene.IsValid())
		return;

	if (!WSS->IsWorldStateMatches(PlayCutsceneAction.OnlyAtWorldState))
		return;
	
	auto Cutscene = PlayCutsceneAction.Cutscene.LoadSynchronous();
	FMovieSceneSequencePlaybackSettings CutsceneSettings;
	ALevelSequenceActor* LaunchedCutscene = nullptr;
	auto ActiveCutScenePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(GetWorld(), Cutscene, CutsceneSettings, LaunchedCutscene);
	SetActiveCutsceneData(PlayCutsceneAction, ActiveCutScenePlayer);
	ActiveCutScenePlayer->OnFinished.AddDynamic(this, &UQuestSubsystem::OnCutsceneFinished);
	ActiveCutScenePlayer->OnStop.AddDynamic(this, &UQuestSubsystem::OnCutsceneFinished);
	ActiveCutScenePlayer->Play();
	if (CutsceneStartedEvent.IsBound())
		CutsceneStartedEvent.Broadcast();
}

void UQuestSubsystem::TrySetLevelStates(const TArray<FQuestSetSublevelStateAction>& SublevelActions,
	const UWorldStateSubsystem* WSS, bool bLoad) const
{
	if (SublevelActions.Num() <= 0 || !SublevelStateChangedEvent.IsBound())
		return;
	
	for (const auto& SublevelAction : SublevelActions)
	{
		if (SublevelAction.bLoad == bLoad && WSS->IsWorldStateMatches(SublevelAction.OnlyAtWorldState))
			SublevelStateChangedEvent.Broadcast(SublevelAction.Level, SublevelAction.bLoad);
	}
}

void UQuestSubsystem::TryDestroyAllActorsWithGameplayTags(const TArray<FDestroyActorsAction>& DestroyActorsActions) const
{
	for (const auto& DestroyActorsAction : DestroyActorsActions)
	{
		if (DestroyActorsAction.ActorClass)
		{
			for (TActorIterator<AActor> ActorIterator(GetWorld(), DestroyActorsAction.ActorClass); ActorIterator; ++ActorIterator)
			{
				bool bDestroy = true;
				if (!DestroyActorsAction.GameplayTagFilter.IsEmpty())
				{
					auto GameplayTagActor = Cast<IGameplayTagAssetInterface>(*ActorIterator);
					if (GameplayTagActor)
					{
						FGameplayTagContainer GameplayTagContainer;
						GameplayTagActor->GetOwnedGameplayTags(GameplayTagContainer);
						bDestroy = DestroyActorsAction.GameplayTagFilter.Matches(GameplayTagContainer);
					}
					else bDestroy = false;
				}

				if (bDestroy)
				{
					if (DestroyActorsAction.OverTime > 0.f)
						ActorIterator->SetLifeSpan(DestroyActorsAction.OverTime);
					else
						ActorIterator->Destroy();
				}
			}
		}
	}
	
	// if (DestroyActorsAction.GameplayTagFilter.IsEmpty())
	// 	return;
	//
	//
	// TArray<AActor*> Actors;
	// UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UGameplayTagAssetInterface::StaticClass(), Actors);
	// if (Actors.Num() <= 0)
	// 	return;
	//
	// for (AActor* Actor : Actors)
	// {
	// 	IGameplayTagAssetInterface* GameplayTagActor = (IGameplayTagAssetInterface*)(Actor);
	// 	FGameplayTagContainer GameplayTagContainer;
	// 	GameplayTagActor->GetOwnedGameplayTags(GameplayTagContainer);
	// 	if (DestroyActorsAction.GameplayTagFilter.Matches(GameplayTagContainer))
	// 	{
	// 		if (DestroyActorsAction.OverTime > 0.f)
	// 			Actor->SetLifeSpan(DestroyActorsAction.OverTime);
	// 		else
	// 			Actor->Destroy();
	// 	}
	// }
}

void UQuestSubsystem::TryRunNpcBehaviors(const TArray<FQuestActionNpcRunBehavior>& RunNpcBehaviorsActions, const UWorldStateSubsystem* WSS) const
{
	auto NpcSubsystem = GetWorld()->GetSubsystem<UNpcSubsystem>();
	for (const auto& RunNpcBehaviorAction : RunNpcBehaviorsActions)
		if (WSS->IsWorldStateMatches(RunNpcBehaviorAction.OnlyAtWorldState))
			NpcSubsystem->TryRunQuestBehavior(RunNpcBehaviorAction);
}

void UQuestSubsystem::SetActiveCutsceneData(const FQuestPlayCutsceneAction& PlayCutsceneAction, ULevelSequencePlayer* ActiveCutscenePlayer)
{
	ActiveCutsceneData.ActiveCutscenePlayer = ActiveCutscenePlayer;
	ActiveCutsceneData.bUnskippable = PlayCutsceneAction.bUnskippable;
	ActiveCutsceneData.bPlayerLocked = PlayCutsceneAction.bLockCharacter;
}

#pragma endregion QUEST ACTIONS

bool UQuestSubsystem::IsQuestTaskPassItemFilter(const FQuestRequirementItemFilter& ItemsFilter) const
{
	if (!ItemsFilter.bActive)
		return true;
	
	UInventoryComponent* InventoryComponent = PlayerCharacter->GetInventoryComponent();
	bool bCanCompleteQuestEvent = true;
	if (ItemsFilter.MustPossessItems.Num() > 0)
	{
		switch (ItemsFilter.ContainmentType)
		{
		case EContainmentType::Any:
			bCanCompleteQuestEvent = false;
			for (const auto& WorldItemDTRH : ItemsFilter.MustPossessItems)
			{
				if (InventoryComponent->HasItem(WorldItemDTRH) == ItemsFilter.bMustPossess)
				{
					bCanCompleteQuestEvent = true;
					break;
				}
			}
			break;
		case EContainmentType::All:
			for (const auto& WorldItemDTRH : ItemsFilter.MustPossessItems)
			{
				if (InventoryComponent->HasItem(WorldItemDTRH) != ItemsFilter.bMustPossess)
				{
					bCanCompleteQuestEvent = false;
					break;
				}
			}
			break;
		default:
			break;
		}	
	}

	return bCanCompleteQuestEvent;
}

void UQuestSubsystem::OnCutsceneFinished()
{
	if (CutsceneEndedEvent.IsBound())
		CutsceneEndedEvent.Broadcast();
}

void UQuestSubsystem::HandleCoveredTasks()
{
	auto WorldState = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>()->GetWorldState();
	TArray<FQuestProgress> QuestsToComplete;
	for (auto& ActiveQuest : ActiveQuests)
	{
		TArray<FQuestTaskInfo> CoveredQuestTasks;
		for (auto& PendingQuestTask : ActiveQuest.PendingQuestTasks)
		{
			FQuestTaskDTR* QuestTaskDTR = PendingQuestTask.QuestTaskDTRH.GetRow<FQuestTaskDTR>("");
			if (!QuestTaskDTR->CoveredByWorldState.IsEmpty() && QuestTaskDTR->CoveredByWorldState.Matches(WorldState))
				CoveredQuestTasks.Add(PendingQuestTask);
		}

		CompleteQuestTasks(ActiveQuest, QuestsToComplete, CoveredQuestTasks, true);
	}

	CompleteQuests(QuestsToComplete);
}

void UQuestSubsystem::TrySkipCutscene() const
{
	if (ActiveCutsceneData.ActiveCutscenePlayer.IsValid() && ActiveCutsceneData.ActiveCutscenePlayer->IsPlaying() && !ActiveCutsceneData.bUnskippable)
		ActiveCutsceneData.ActiveCutscenePlayer->GoToEndAndStop();
}

FJournalModel UQuestSubsystem::GetJournalData()
{
	FJournalModel Result;

	auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
	
	for (const auto& QuestData : ActiveQuests)
	{
		FQuestItem ActiveQuestInfo;
		FQuestDTR* QuestDTR = QuestData.QuestDTRH.GetRow<FQuestDTR>("");
		ActiveQuestInfo.Title = QuestDTR->Name;
		ActiveQuestInfo.Description = QuestDTR->Description;
		ActiveQuestInfo.IsAuxiliary = QuestDTR->IsAuxiliary;
		ActiveQuestInfo.IsCompleted = false;
		ActiveQuestInfo.Id = QuestData.QuestDTRH.RowName;
		
		for (const auto& QuestTaskInfo : QuestData.PendingQuestTasks)
		{
			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			if (QuestTaskDTR->bImplicit || !QuestTaskDTR->bActive
				|| !WSS->IsWorldStateMatches(QuestTaskDTR->QuestTaskRequirements.RequiresWorldState))
				continue;
			
			FSubtaskItem SubtaskItem;
			SubtaskItem.Title = QuestTaskDTR->Title;
			SubtaskItem.IsCompleted = false;
			ActiveQuestInfo.Subtasks.Add(SubtaskItem);
		}

		for (const auto& QuestTaskInfo : QuestData.CompletedQuestTasks)
		{
			auto QuestTaskDTR = QuestTaskInfo.GetQuestTaskDTR();
			if (QuestTaskDTR->bImplicit || !QuestTaskDTR->bActive)
				continue;
			
			FSubtaskItem SubtaskItem;
			SubtaskItem.Title = QuestTaskDTR->Title;
			SubtaskItem.IsCompleted = true;
			ActiveQuestInfo.Subtasks.Add(SubtaskItem);
		}
		
		Result.Quests.Add(ActiveQuestInfo);
	}

	for (const auto& QuestData : CompletedQuests)
	{
		FQuestItem CompletedQuestInfo;
		FQuestDTR* QuestDTR = QuestData.QuestDTRH.GetRow<FQuestDTR>("");
		
		CompletedQuestInfo.Title = QuestDTR->Name;
		CompletedQuestInfo.Description = QuestDTR->Description;
		CompletedQuestInfo.IsAuxiliary = QuestDTR->IsAuxiliary;
		CompletedQuestInfo.IsCompleted = true;

		Result.Quests.Add(CompletedQuestInfo);
	}
	
	return Result;
}

FQuestNavigationGuidance UQuestSubsystem::GetNavigationGuidance(const FName& RowName)
{
	FQuestNavigationGuidance Result;
	
	for (const auto& ActiveQuest : ActiveQuests)
	{
		if (ActiveQuest.QuestDTRH.RowName == RowName)
		{
			auto WSS = GetGameInstance()->GetSubsystem<UWorldStateSubsystem>();
			auto WLS = GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();

			auto QuestDTR = ActiveQuest.QuestDTRH.GetRow<FQuestDTR>("");
			for (const auto& NavigationGuidance : QuestDTR->NavigationGuidances)
			{
				if (WSS->IsWorldStateMatches(NavigationGuidance.AtWorldState))
				{
					Result.QuestLocation = WLS->GetClosestQuestLocationSimple(NavigationGuidance.LocationDTRH, PlayerCharacter->GetActorLocation());
					Result.UntilWorldState = NavigationGuidance.UntilWorldState;
					return Result;
				}	
			}
		}
	}

	return Result;
}

const AQuestLocation* UQuestSubsystem::GetQuestLocation(const FDataTableRowHandle& SpawnLocationDTRH) const
{
	auto WLS = GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WLS)
		return nullptr;

	return WLS->GetWorldLocation(SpawnLocationDTRH);
}
