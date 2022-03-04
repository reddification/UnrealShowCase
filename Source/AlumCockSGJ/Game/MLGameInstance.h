#pragma once

#include "CoreMinimal.h"
#include "MLCoreTypes.h"
#include "Data/Quests/QuestAction.h"
#include "Engine/GameInstance.h"
#include "MLGameInstance.generated.h"

class AQuestLocation;
class UObservationSourceComponent;


UCLASS()
class ALUMCOCKSGJ_API UMLGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
    FJournalModel Model;
    
    virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
    FName GetStartupLevelName() const { return StartupLevelName; }
    FName GetMenuLevelName() const { return MenuLevelName; };
    
protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName StartupLevelName = NAME_None;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName MenuLevelName = NAME_None;
};
