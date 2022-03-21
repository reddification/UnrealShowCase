#include "WorldLocationsSubsystem.h"

#include "NavigationSystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Kismet/GameplayStatics.h"

void UWorldLocationsSubsystem::RegisterWorldLocation(AQuestLocation* QuestLocation)
{
	if (!IsValid(QuestLocation))
		return;
	
	QuestLocationsMap.Add(QuestLocation->GetLocationDTRH().RowName, QuestLocation);
}

void UWorldLocationsSubsystem::UnregisterWorldLocation(const FDataTableRowHandle& LocationDTRH)
{
	QuestLocationsMap.Remove(LocationDTRH.RowName);
}

void UWorldLocationsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

AQuestLocation* UWorldLocationsSubsystem::GetWorldLocationRandom(const FDataTableRowHandle& QuestLocationDTRH)
{
	TArray<AQuestLocation*> QuestLocationsArray;
	QuestLocationsMap.MultiFind(QuestLocationDTRH.RowName, QuestLocationsArray);
	if (QuestLocationsArray.Num() == 0)
		return nullptr;

	return QuestLocationsArray[FMath::RandRange(0, QuestLocationsArray.Num() - 1)];
}

const AQuestLocation* UWorldLocationsSubsystem::GetClosestQuestLocationSimple(const FDataTableRowHandle& QuestLocationDTRH,
	const FVector& QuerierLocation)
{
	TArray<AQuestLocation*> QuestLocationsArray;
	QuestLocationsMap.MultiFind(QuestLocationDTRH.RowName, QuestLocationsArray);
	if (QuestLocationsArray.Num() == 1)
		return IsValid(QuestLocationsArray[0]) ? QuestLocationsArray[0] : nullptr;
	
	float ClosestSquaredDistance = FLT_MAX;
	const AQuestLocation* FoundLocation = nullptr;
	for (const auto QuestLocation : QuestLocationsArray)
	{
		if (!IsValid(QuestLocation))
			continue;
		
		float SquaredDistance = FVector::DistSquared(QuestLocation->GetActorLocation(), QuerierLocation);
		if (SquaredDistance < ClosestSquaredDistance)
		{
			FoundLocation = QuestLocation;
			ClosestSquaredDistance = SquaredDistance;
		}
	}

	return FoundLocation;
}

const AQuestLocation* UWorldLocationsSubsystem::GetClosestQuestLocationComplex(const FDataTableRowHandle& QuestLocationDTRH,
	const FVector& QuerierLocation, UObject* WorldContextObject)
{
	TArray<AQuestLocation*> QuestLocationsArray;
	QuestLocationsMap.MultiFind(QuestLocationDTRH.RowName, QuestLocationsArray);
	if (QuestLocationsArray.Num() == 1)
		return QuestLocationsArray[0];
	
	float ClosestDistance = FLT_MAX;
	const AQuestLocation* FoundLocation = nullptr;
	for (const auto QuestLocation : QuestLocationsArray)
	{
		float TestedDistance = FLT_MAX;
		auto PathLengthRetrieveResult = UNavigationSystemV1::GetPathLength(WorldContextObject, QuerierLocation,
			QuestLocation->GetActorLocation(), TestedDistance);
		// if (PathLengthRetrieveResult == ENavigationQueryResult::Success)
		// {
			if (TestedDistance < ClosestDistance)
			{
				FoundLocation = QuestLocation;
				ClosestDistance = TestedDistance;
			}
		// }
	}

	return FoundLocation;
}