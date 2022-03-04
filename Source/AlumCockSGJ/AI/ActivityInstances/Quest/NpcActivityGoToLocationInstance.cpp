#include "NpcActivityGoToLocationInstance.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "Subsystems/WorldLocationsSubsystem.h"

void UNpcActivityGoToLocationInstance::Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor,
                                             AAIController* AIController)
{
	Super::Setup(NpcBehaviorDescriptor, AIController);
	QuestSettings = Cast<UNpcActivityGoToLocationSettings>(NpcBehaviorDescriptor.NpcActivitySettings);
}

bool UNpcActivityGoToLocationInstance::FindNextLocation(const AAIController* AIController,
	FNextLocationData& OutNextLocationData)
{
	if (!QuestSettings || QuestSettings->GoToLocations.Num() <= 0)
		return false;
	
	auto WLS = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	const auto& RandomLocation = QuestSettings->GoToLocations[FMath::RandRange(0, QuestSettings->GoToLocations.Num() - 1)];
	auto WorldLocation = WLS->GetClosestQuestLocationSimple(RandomLocation,AIController->GetPawn()->GetActorLocation());
	if (!WorldLocation)
		return false;

	OutNextLocationData.NextLocation = WorldLocation->GetActorLocation();
	return true;
}
