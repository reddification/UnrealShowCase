#include "NpcActivityIndefinetilyStayAtLocation.h"

#include "Actors/Interactive/Environment/QuestLocation.h"
#include "AI/Interfaces/TraversalActivity.h"
#include "Subsystems/WorldLocationsSubsystem.h"

void UNpcActivityIndefinetilyStayAtLocation::Setup(const FNpcBehaviorActivityDescriptor& NpcBehaviorDescriptor,
                                                   AAIController* AIController)
{
	Super::Setup(NpcBehaviorDescriptor, AIController);
	Settings = Cast<UNpcActivityIndefinetilyStayAtLocationSettings>(NpcBehaviorDescriptor.NpcActivitySettings);	
}

bool UNpcActivityIndefinetilyStayAtLocation::FindNextLocation(const AAIController* AIController,
	FNextLocationData& OutNextLocationData)
{
	auto WLS = AIController->GetWorld()->GetSubsystem<UWorldLocationsSubsystem>();
	auto WorldLocation = WLS->GetClosestQuestLocationSimple(Settings->BaseActivityLocationDTRH, AIController->GetPawn()->GetActorLocation());
	if (IsValid(WorldLocation))
	{
		OutNextLocationData.NextLocation = WorldLocation->GetActorLocation();
		return true;
	}

	return false;
}

FGameplayTag UNpcActivityIndefinetilyStayAtLocation::GetStateTag() const
{
	return Settings->NpcStateWhenGoingToSpot;
}

FGameplayTag UNpcActivityIndefinetilyStayAtLocation::GetReactionTag() const
{
	if (Settings->NpcReactInSpotOptions.Num() <= 0)
		return FGameplayTag::EmptyTag;
	
	return Settings->NpcReactInSpotOptions[FMath::RandRange(0, Settings->NpcReactInSpotOptions.Num() - 1)];
}
