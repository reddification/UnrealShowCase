#include "NpcActivityWanderAroundInstance.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Data/NpcActivityBaseSettings.h"
#include "Data/LogTypes.h"
#include "Game/GCGameInstance.h"
#include "Subsystems/WorldLocationsSubsystem.h"

bool UNpcActivityWanderAroundInstance::FindNextLocation(const AAIController* AIController,
	FNextLocationData& NextLocationData)
{
	auto WorldLocationSubsystem = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WorldLocationSubsystem)
		return false;
	
	auto Settings = GetSettings();
	if (!IsValid(Settings))
		return false;
	
	FWanderAroundLocationDescriptor LocationDescriptor;
	if (Settings->bRandomPick)
	{
		int RandomIndex = FMath::RandRange(0, Settings->LocationsDescriptors.Num() - 1);
		LastLocationIndex = RandomIndex;
		LocationDescriptor = Settings->LocationsDescriptors[RandomIndex];
	}
	else
	{
		LastLocationIndex = (LastLocationIndex + 1) % Settings->LocationsDescriptors.Num();
		LocationDescriptor = Settings->LocationsDescriptors[LastLocationIndex];
	}

	auto AICharacter = AIController->GetPawn();
	auto Location = WorldLocationSubsystem->GetClosestQuestLocationComplex(LocationDescriptor.LocationDTRH,
		AICharacter->GetActorLocation(), AICharacter->GetWorld());
	if (!Location)
	{
		// UE_LOG(LogNpcActivity, Log, TEXT("[%s %s] Couldn't find WorldLocation %s"), *AIController->GetName(),
		// 	*GET_FUNCTION_NAME_CHECKED(UNpcActivityWanderAroundInstance, FindNextLocation).ToString(),
		// 	*LocationDescriptor.LocationDTRH.ToDebugString());
		return false;
	}
	
	NextLocationData.NextLocation = Location->GetActorLocation();
	NextLocationData.RecommendedStayTime = LocationDescriptor.StayTime;
	// UE_LOG(LogNpcActivity, Log, TEXT("[%s %s] Found next location to go: %s"), *AIController->GetName(),
	// 		*GET_FUNCTION_NAME_CHECKED(UNpcActivityWanderAroundInstance, FindNextLocation).ToString(),
	// 		*NextLocationData.NextLocation.ToString());
	return true;
}

bool UNpcActivityWanderAroundInstance::AssessActivityState(AAIController* AIController,
	UBlackboardComponent* Blackboard)
{
	auto WorldLocationSubsystem = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	if (!WorldLocationSubsystem)
		return false;
	
	auto Settings = GetSettings();
	if (!IsValid(Settings))
		return false;
	
	const FWanderAroundLocationDescriptor& LocationDescriptor = Settings->LocationsDescriptors[LastLocationIndex];
	auto Location = WorldLocationSubsystem->GetClosestQuestLocationSimple(LocationDescriptor.LocationDTRH,
		AIController->GetPawn()->GetActorLocation());

	if (!Location)
		return false;

	AIController->GetPawn()->SetActorRotation(Location->GetActorRotation());
	return true;
}

const UNpcActivityWanderAroundSettings* UNpcActivityWanderAroundInstance::GetSettings() const
{
	if (!WanderAroundSettings)
		WanderAroundSettings = Cast<UNpcActivityWanderAroundSettings>(ActivitySettings);

	return WanderAroundSettings;
}

