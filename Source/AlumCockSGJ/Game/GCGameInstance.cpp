#include "GCGameInstance.h"
#include "DebugSubsystem.h"
#include "NavigationSystem.h"
#include "Actors/Interactive/Environment/QuestLocation.h"
#include "Actors/Interactive/Pickables/BasePickableItem.h"
#include "Data/Entities/WorldItemDTR.h"
#include "Interfaces/DatatableInitializablePawn.h"
#include "Kismet/GameplayStatics.h"

bool UGCGameInstance::ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor)
{
	bool bResult = Super::ProcessConsoleExec(Cmd, Ar, Executor);
	if (!bResult)
	{
		TArray<UGameInstanceSubsystem*> Subsystems = GetSubsystemArray<UGameInstanceSubsystem>();
		for (const auto Subsystem : Subsystems)
		{
			if (Subsystem->ProcessConsoleExec(Cmd, Ar, Executor))
				break;
		}
	}
	
	return bResult;
}

void UGCGameInstance::LoadQuestLocations()
{
	TArray<AActor*> QuestLocationsActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AQuestLocation::StaticClass(), QuestLocationsActors);
	QuestLocations.Reset();
	for (AActor* QuestLocationActor : QuestLocationsActors)
	{
		AQuestLocation* QuestLocation = Cast<AQuestLocation>(QuestLocationActor);
		if (IsValid(QuestLocation))
		{
			QuestLocations.Add(QuestLocation);
			TArray<AQuestLocation*> QuestLocationArray;
			QuestLocationsMap.Add(QuestLocation->GetLocationDTRH().RowName, QuestLocation);
		}
	}
}

void UGCGameInstance::SpawnEnemies(const FQuestActionSpawnEnemies& SpawnEnemiesData, const FActorSpawnParameters& ActorSpawnParameters,
	const FVector& PlayerLocation) const
{
	const AQuestLocation* QuestLocation = GetQuestLocation(SpawnEnemiesData.SpawnLocationDTRH);
	if (!IsValid(QuestLocation))
		return;
	
	for (auto i = 0; i < SpawnEnemiesData.Count; i++)
	{
		FVector SpawnLocation = QuestLocation->GetRandomLocationInVolume(SpawnEnemiesData.FloorOffset);
		APawn* SpawnedPawn = GetWorld()->SpawnActor<APawn>(SpawnEnemiesData.EnemyClass, SpawnLocation,
			FRotator::ZeroRotator, ActorSpawnParameters);
		if (!SpawnEnemiesData.NpcDTRH.IsNull())
		{
			auto DatatableInitializablePawn = Cast<IDatatableInitializablePawn>(SpawnedPawn);
			if (DatatableInitializablePawn)
				DatatableInitializablePawn->SetNpcDTRH(SpawnEnemiesData.NpcDTRH);
		}
		
		if (!SpawnEnemiesData.WithTag_Old.IsNone())
			SpawnedPawn->Tags.Add(SpawnEnemiesData.WithTag_Old);
			
		SpawnedPawn->SpawnDefaultController();
		if (SpawnEnemiesData.InitialInterest != ENpcInitialInterest::None)
		{
			auto AIController = Cast<AAIBaseController>(SpawnedPawn->Controller);
			if (IsValid(AIController))
			{
				switch (SpawnEnemiesData.InitialInterest)
				{
				case ENpcInitialInterest::Player:
					AIController->SetInterestingLocation(PlayerLocation);
					break;
				case ENpcInitialInterest::Location:
					{
						auto TargetLocation = GetQuestLocation(SpawnEnemiesData.InterestingLocationDTRH);
						if (IsValid(TargetLocation))
							AIController->SetInterestingLocation(TargetLocation->GetActorLocation());
					}
					break;
				default:
					break;
				}
			}	
		}
	}
}

FVector UGCGameInstance::GetRandomNavigableLocationNearPlayer(const FVector& PlayerLocation, float Radius, float FloorOffset) const
{
	FVector OutLocation;
	bool bLocationFound = UNavigationSystemV1::K2_GetRandomReachablePointInRadius(GetWorld(), PlayerLocation,
		OutLocation, Radius);

	return (bLocationFound ? OutLocation : PlayerLocation) + FVector::UpVector * FloorOffset;
}

void UGCGameInstance::SpawnItems(const FQuestActionSpawnItems& SpawnItemData, const FActorSpawnParameters& ActorSpawnParameters, const FVector& PlayerLocation) const
{
	AQuestLocation* TargetQuestLocation = GetQuestLocation(SpawnItemData.SpawnLocationDTRH);
	if (!IsValid(TargetQuestLocation))
		return;

	FWorldItemDTR* WorldItemDTR = SpawnItemData.WorldItemDTRH.DataTable->FindRow<FWorldItemDTR>(SpawnItemData.WorldItemDTRH.RowName, "");
	if (!WorldItemDTR)
		return;
		
	for (auto i = 0; i < SpawnItemData.Count; i++)
	{
		FVector SpawnLocation = SpawnItemData.bNearPlayer
			? GetRandomNavigableLocationNearPlayer(PlayerLocation, SpawnItemData.NearPlayerRadius, SpawnItemData.FloorOffset)
			: TargetQuestLocation->GetRandomLocationInVolume(SpawnItemData.FloorOffset);
		if (!IsValid(WorldItemDTR->SpawnWorldItemClass))
			continue;
			
		ABaseWorldItem* SpawnedPickable = GetWorld()->SpawnActor<ABaseWorldItem>(WorldItemDTR->SpawnWorldItemClass,
			SpawnLocation, FRotator::ZeroRotator, ActorSpawnParameters);
		SpawnedPickable->SetItemDTRH(SpawnItemData.WorldItemDTRH);
		if (!SpawnItemData.WithTag_Old.IsNone())
			SpawnedPickable->Tags.Add(SpawnItemData.WithTag_Old);
	}
}

AQuestLocation* UGCGameInstance::GetQuestLocation(const FDataTableRowHandle& QuestLocationDTRH) const
{
	for (const auto QuestLocation : QuestLocations)
	{
		if (QuestLocation->GetLocationDTRH() == QuestLocationDTRH)
			return QuestLocation;
	}

	GEngine->AddOnScreenDebugMessage(90, 5, FColor::Emerald,
	FString::Printf(TEXT("Can't find quest location %s"), *QuestLocationDTRH.ToDebugString()));
	return nullptr;
}

const AQuestLocation* UGCGameInstance::GetClosestQuestLocationSimple(const FDataTableRowHandle& QuestLocationDTRH,
	const FVector& QuerierLocation) const
{
	TArray<AQuestLocation*> QuestLocationsArray;
	QuestLocationsMap.MultiFind(QuestLocationDTRH.RowName, QuestLocationsArray);
	if (QuestLocationsArray.Num() == 1)
		return QuestLocationsArray[0];
	
	float ClosestSquaredDistance = FLT_MAX;
	const AQuestLocation* FoundLocation = nullptr;
	for (const auto QuestLocation : QuestLocationsArray)
	{
		float SquaredDistance = FVector::DistSquared(QuestLocation->GetActorLocation(), QuerierLocation);
		if (SquaredDistance < ClosestSquaredDistance)
		{
			FoundLocation = QuestLocation;
			ClosestSquaredDistance = SquaredDistance;
		}
	}

	return FoundLocation;
}

const AQuestLocation* UGCGameInstance::GetClosestQuestLocationComplex(const FDataTableRowHandle& QuestLocationDTRH,
	const FVector& QuerierLocation, UObject* WorldContextObject) const
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
