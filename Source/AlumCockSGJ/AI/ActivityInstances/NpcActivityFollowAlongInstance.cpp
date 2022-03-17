#include "NpcActivityFollowAlongInstance.h"

#include "AI/Interfaces/NpcCharacter.h"

void UNpcActivityFollowAlongInstance::Resume()
{
	Super::Resume();
	PredictedLocation = FAISystem::InvalidLocation;
	FollowedActor = nullptr;
}

float UNpcActivityFollowAlongInstance::Suspend(AAIController* AIController, bool bAbortInteraction)
{
	auto Npc = Cast<INpcCharacter>(AIController->GetPawn());
	Npc->ResetDesiredSpeed();
	return Super::Suspend(AIController, bAbortInteraction);
}

void UNpcActivityFollowAlongInstance::SetFollowedActor(APawn* NewFollowedActor)
{
	FollowedActor = NewFollowedActor;
}

void UNpcActivityFollowAlongInstance::SetPredictedLocation(FVector NewPredictedLocation)
{
	PredictedLocation = NewPredictedLocation;
}