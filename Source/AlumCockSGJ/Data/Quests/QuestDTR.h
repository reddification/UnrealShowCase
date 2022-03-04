#pragma once
#include "QuestAction.h"
#include "QuestDialogue.h"
#include "QuestRequirements.h"
#include "QuestTypes.h"
#include "Engine/DataTable.h"
#include "QuestDTR.generated.h"

USTRUCT(BlueprintType)
struct FQuestDTR : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Name;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(MultiLine = true))
	FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    bool IsAuxiliary;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(RequiredAssetDataTags="RowStructure=QuestTaskDTR"))
	class UDataTable* QuestEventsDT;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestRequirements QuestRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FQuestActions BeginQuestActions;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FQuestActions EndQuestActions;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FQuestDialogue InitialDialogue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FQuestDialogue EndDialogue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FQuestNavigationGuidanceData> NavigationGuidances;
};
