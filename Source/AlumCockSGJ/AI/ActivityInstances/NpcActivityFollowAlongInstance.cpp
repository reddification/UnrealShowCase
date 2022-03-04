#include "NpcActivityFollowAlongInstance.h"

#include "AI/Interfaces/NpcCharacter.h"

void UNpcActivityFollowAlongInstance::Resume()
{
	Super::Resume();
	PredictedLocation = FAISystem::InvalidLocation;
	FollowedActor = nullptr;
}

void UNpcActivityFollowAlongInstance::Suspend(AAIController* AIController, bool bAbortInteraction)
{
	Super::Suspend(AIController, bAbortInteraction);
	auto Npc = Cast<INpcCharacter>(AIController->GetPawn());
	Npc->ResetDesiredSpeed();
}

void UNpcActivityFollowAlongInstance::SetFollowedActor(APawn* NewFollowedActor)
{
	FollowedActor = NewFollowedActor;
}

void UNpcActivityFollowAlongInstance::SetPredictedLocation(FVector NewPredictedLocation)
{
	PredictedLocation = NewPredictedLocation;
}