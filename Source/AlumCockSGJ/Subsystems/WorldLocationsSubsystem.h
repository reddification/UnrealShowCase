#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "UObject/Object.h"
#include "WorldLocationsSubsystem.generated.h"

class AQuestLocation;
UCLASS()
class ALUMCOCKSGJ_API UWorldLocationsSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	AQuestLocation* GetWorldLocationRandom(const FDataTableRowHandle& QuestLocationDTRH);
    
	const AQuestLocation* GetClosestQuestLocationSimple(const FDataTableRowHandle& QuestLocationDTRH,
														const FVector& QuerierLocation);
	const AQuestLocation* GetClosestQuestLocationComplex(const FDataTableRowHandle& QuestLocationDTRH,
														 const FVector& QuerierLocation,
														 UObject* WorldContextObject);
	
	void RegisterWorldLocation(AQuestLocation* QuestLocation);
	void UnregisterWorldLocation(const FDataTableRowHandle& LocationDTRH);

private:
	// TODO leave only TMap and its related functions
	// TArray<AQuestLocation*> QuestLocations;

	TMultiMap<FName, AQuestLocation*> QuestLocationsMap;
};
