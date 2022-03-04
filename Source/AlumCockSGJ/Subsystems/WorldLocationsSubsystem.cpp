#include "WorldLocationsSubsystem.h"

#include "NavigationSystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Kismet/GameplayStatics.h"

void UWorldLocationsSubsystem::CacheQuestLocations()
{
	// TArray<AActor*> QuestLocationsActors;
	// auto World = GetWorld();
	// UGameplayStatics::GetAllActorsOfClass(World, AQuestLocation::StaticClass(), QuestLocationsActors);
	//
	// QuestLocations.Reset();
	// for (AActor* QuestLocationActor : QuestLocationsActors)
	// {
	// 	AQuestLocation* QuestLocation = Cast<AQuestLocation>(QuestLocationActor);
	// 	if (IsValid(QuestLocation))
	// 	{
	// 		QuestLocations.Add(QuestLocation);
	// 		QuestLocationsMap.Add(QuestLocation->GetLocationDTRH().RowName, QuestLocation);
	// 	}
	// }
}

void UWorldLocationsSubsystem::RegisterWorldLocation(AQuestLocation* QuestLocation)
{
	if (!IsValid(QuestLocation))
		return;
	
	QuestLocations.Add(QuestLocation);
	QuestLocationsMap.Add(QuestLocation->GetLocationDTRH().RowName, QuestLocation);
}

void UWorldLocationsSubsystem::UnregisterWorldLocation(const FDataTableRowHandle& LocationDTRH)
{
	QuestLocations.RemoveAllSwap([LocationDTRH](AQuestLocation* QuestLocation)
	{
		return IsValid(QuestLocation)
			? QuestLocation->GetLocationDTRH() == LocationDTRH
			: true;
	});

	QuestLocationsMap.Remove(LocationDTRH.RowName);
}



void UWorldLocationsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	CacheQuestLocations();
}

AQuestLocation* UWorldLocationsSubsystem::GetWorldLocation(const FDataTableRowHandle& QuestLocationDTRH)
{
	if (QuestLocations.Num() == 0)
		CacheQuestLocations();
	
	for (const auto QuestLocation : QuestLocations)
	{
		if (QuestLocation->GetLocationDTRH() == QuestLocationDTRH)
			return QuestLocation;
	}

	GEngine->AddOnScreenDebugMessage(90, 5, FColor::Emerald,
	FString::Printf(TEXT("Can't find quest location %s"), *QuestLocationDTRH.ToDebugString()));
	return nullptr;
}

const AQuestLocation* UWorldLocationsSubsystem::GetClosestQuestLocationSimple(const FDataTableRowHandle& QuestLocationDTRH,
	const FVector& QuerierLocation)
{
	if (QuestLocations.Num() == 0)
		CacheQuestLocations();
	
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
	if (QuestLocations.Num() == 0)
		CacheQuestLocations();
	
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