#pragma once

#include "CoreMinimal.h"
#include "Data/Quests/QuestAction.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "GCGameInstance.generated.h"

class AQuestLocation;
class UObservationSourceComponent;

UCLASS()
class ALUMCOCKSGJ_API UGCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
    FName GetStartupLevelName() const { return StartupLevelName; }
    FName GetMenuLevelName() const { return MenuLevelName; };
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName StartupLevelName = NAME_None;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName MenuLevelName = NAME_None;
};