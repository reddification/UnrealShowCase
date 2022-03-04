#pragma once

#include "CoreMinimal.h"
#include "Data/Quests/QuestAction.h"
#include "Engine/DataTable.h"
#include "Engine/GameInstance.h"
#include "GCGameInstance.generated.h"

class AQuestLocation;
class UObservationSourceComponent;

// TODO split into subsystems
UCLASS()
class ALUMCOCKSGJ_API UGCGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;
    FName GetStartupLevelName() const { return StartupLevelName; }
    FName GetMenuLevelName() const { return MenuLevelName; };
    
    void LoadQuestLocations();
    void SpawnEnemies(const FQuestActionSpawnEnemies& SpawnEnemiesData,
                      const FActorSpawnParameters& ActorSpawnParameters, const FVector& PlayerLocation) const;
    void SpawnItems(const FQuestActionSpawnItems& SpawnItemData,
                    const FActorSpawnParameters& ActorSpawnParameters, const FVector& PlayerLocation) const;

    AQuestLocation* GetQuestLocation(const FDataTableRowHandle& QuestLocationDTRH) const;
    
    const AQuestLocation* GetClosestQuestLocationSimple(const FDataTableRowHandle& QuestLocationDTRH,
                                                        const FVector& QuerierLocation) const;
    const AQuestLocation* GetClosestQuestLocationComplex(const FDataTableRowHandle& QuestLocationDTRH,
                                                         const FVector& QuerierLocation,
                                                         UObject* WorldContextObject) const;


protected:
    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName StartupLevelName = NAME_None;

    UPROPERTY(EditDefaultsOnly, Category = "Game")
    FName MenuLevelName = NAME_None;

private:
    // TODO leave only TMap and its related functions
    TArray<AQuestLocation*> QuestLocations;

    TMultiMap<FName, AQuestLocation*> QuestLocationsMap;
    // TMap<FDataTableRowHandle, FQuestLocationsWrapper> QuestLocationsMap;

    FVector GetRandomNavigableLocationNearPlayer(const FVector& PlayerLocation, float Radius, float FloorOffset) const;
};