#pragma once

#include "LevelSequence.h"
#include "AI/Characters/AICharacter.h"
#include "AI/Data/NpcBehaviorSettings.h"
#include "Engine/DataTable.h"
#include "Objects/QuestCustomActionBase.h"
#include "QuestAction.generated.h"

UENUM(BlueprintType)
enum class ENpcInitialInterest : uint8
{
	None,
	Player,
	Location
};

USTRUCT(BlueprintType)
struct FQuestBaseAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery OnlyAtWorldState;
};

USTRUCT(BlueprintType)
struct FQuestSpawnBaseAction : public FQuestBaseAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bNearPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "bNearPlayer == true"))
	float NearPlayerRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition = "bNearPlayer == false", RowType="QuestLocationDTR"))
	FDataTableRowHandle SpawnLocationDTRH;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName WithTag_Old;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer WithTags;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FloorOffset = 50.f;
};

USTRUCT(BlueprintType)
struct FQuestActionSpawnEnemies : public FQuestSpawnBaseAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ABaseCharacter> EnemyClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ENpcInitialInterest InitialInterest = ENpcInitialInterest::None;

	// A location NPC will go to after spawn if InitialInterest is set accordingly
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (EditCondition = "InitialInterest == ENpcInitialInterest::Location", RowType="QuestLocationDTR"))
	FDataTableRowHandle InterestingLocationDTRH;
};

USTRUCT(BlueprintType)
struct FNpcQuestBehaviorDescriptor
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcBehaviorActivityDescriptor BehaviorDescriptor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery UntilWorldState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTagQuery UntilCharacterState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bBlockFeelings = false;
};

USTRUCT(BlueprintType)
struct FQuestActionNpcRunBehavior : public FQuestBaseAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcQuestBehaviorDescriptor NpcQuestBehaviorDescriptor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag WithTagOnly;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bFirstOnly = false;
};

USTRUCT(BlueprintType)
struct FQuestActionSpawnNpcs : public FQuestSpawnBaseAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	bool bSpawnNew = false;
	
	// optional
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FNpcQuestBehaviorDescriptor NpcQuestBehaviorDescriptor;
};

USTRUCT(BlueprintType)
struct FQuestActionSpawnItems : public FQuestSpawnBaseAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FDataTableRowHandle WorldItemDTRH;
};

USTRUCT(BlueprintType)
struct FQuestSetSublevelStateAction : public FQuestBaseAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLoad = true;
};

USTRUCT(BlueprintType)
struct FQuestPlayCutsceneAction : public FQuestBaseAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<ULevelSequence> Cutscene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bUnskippable;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bLockCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bCharacterInvincible;
};

USTRUCT(BlueprintType)
struct FQuestCustomAction : public FQuestBaseAction
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AQuestCustomActionBase> CustomActionClass;
};

USTRUCT(BlueprintType)
struct FDestroyActorsAction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery GameplayTagFilter;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0.f, ClampMin = 0.f))
	float OverTime = 0.f;
};

USTRUCT(BlueprintType)
struct FQuestActions
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestActionSpawnEnemies> SpawnEnemies;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestActionSpawnItems> SpawnItems;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestActionSpawnNpcs> SpawnNpcs;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestSetSublevelStateAction> SetSublevelsStates;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestPlayCutsceneAction PlayCutscene;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestCustomAction> CustomActions;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FQuestActionNpcRunBehavior> RunNpcBehaviors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FDestroyActorsAction> DestroyActors;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagContainer WorldStateChanges;
};