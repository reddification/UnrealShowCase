#pragma once

#include "QuestTaskDTR.h"
#include "Engine/DataTable.h"
#include "QuestProgress.generated.h"

USTRUCT(BlueprintType)
struct FQuestTaskInfo
{
    GENERATED_BODY()

    UPROPERTY(SaveGame)
    FDataTableRowHandle QuestTaskDTRH;
    
    FQuestTaskDTR* GetQuestTaskDTR() const { return !QuestTaskDTRH.IsNull() ? QuestTaskDTRH.GetRow<FQuestTaskDTR>("") : nullptr; }

    UPROPERTY(SaveGame)
    int CountProgress = 0;
};

USTRUCT(BlueprintType)
struct FQuestProgress
{
    GENERATED_BODY()

    UPROPERTY(SaveGame)
    FDataTableRowHandle QuestDTRH;

    UPROPERTY(SaveGame)
    TArray<FQuestTaskInfo> PendingQuestTasks;

    UPROPERTY(SaveGame)
    TArray<FQuestTaskInfo> CoveredQuestsTasks;
    
    UPROPERTY(SaveGame)
    TArray<FQuestTaskInfo> CompletedQuestTasks;
};

inline bool operator == (const FQuestProgress& First, const FQuestProgress& Other)
{
    return First.QuestDTRH == Other.QuestDTRH;
}

inline bool operator ==(const FQuestTaskInfo& First, const FQuestTaskInfo& Second)
{
    return First.QuestTaskDTRH == Second.QuestTaskDTRH;
}
