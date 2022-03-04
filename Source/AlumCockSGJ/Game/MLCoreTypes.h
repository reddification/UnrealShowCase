#pragma once

#include "Data/Quests/QuestAction.h"
#include "ImageCore/Public/ImageCore.h"
#include "MLCoreTypes.generated.h"

class AQuestLocation;
DECLARE_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHealthChangedSignature, float);

UENUM(BlueprintType)
enum class EMLGameState : uint8
{
    WaitingToStart = 0,
    InProgress,
    Pause,
    GameOver,
    Journal
};

USTRUCT()
struct FQuestLocationsWrapper
{
    GENERATED_BODY()

    UPROPERTY()
    TArray<AQuestLocation*> QuestLocations;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnGameStateChangedSignature, EMLGameState);

USTRUCT(BlueprintType)
struct FSubtaskItem
{
    GENERATED_BODY()
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsCompleted;
};

USTRUCT(BlueprintType)
struct FQuestItem
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Title;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Description;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TWeakObjectPtr<class UTexture2D> Map;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsCompleted;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool IsAuxiliary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FSubtaskItem> Subtasks;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    //FGuid ID = FGuid::NewGuid();
    FName Id;
    
    bool operator==(const FQuestItem& QuestItem) const
    {
        return Id == QuestItem.Id;
    };
};

USTRUCT(BlueprintType)
struct FJournalModel
{
    GENERATED_BODY()
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FQuestItem> Quests;
};
