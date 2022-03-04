#pragma once

#include "Engine/DataTable.h"
#include "QuestRequirements.h"
#include "GameplayTags/Classes/GameplayTagContainer.h"
#include "QuestTaskActor.generated.h"

USTRUCT(BlueprintType)
struct FQuestTaskActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> ActorType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName WithTag_Old;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery WithGameplayTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestRequirementItemFilter ItemPossessionFilter;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle ByCharacter;
};

USTRUCT(BlueprintType)
struct FQuestTaskKillNPCsTrigger
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class ABaseCharacter> ActorType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FName WithTag_Old;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery WithGameplayTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;
};

USTRUCT(BlueprintType)
struct FQuestTaskItemCollectionTrigger
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle ByCharacterDTRH;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="WorldItemDTR"))
	FDataTableRowHandle WorldItemDTRH;

	// TODO
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTagQuery WithTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int Count = 1;
};

USTRUCT(BlueprintType)
struct FQuestTaskNpcInteraction
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RowType="NpcDTR"))
	FDataTableRowHandle NpcDTRH;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagQuery NpcWithGameplayTags;
};