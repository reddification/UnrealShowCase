#include "NpcDogLogicComponent.h"

#include "AI/Characters/AIDogCharacter2.h"

void UNpcDogLogicComponent::BeginPlay()
{
	DogCharacter = Cast<AAIDogCharacter2>(GetOwner());
	Super::BeginPlay();
}

void UNpcDogLogicComponent::ApplyState(const UBaseNpcStateSettings* NpcState)
{
	Super::ApplyState(NpcState);
	auto DogState = StaticCast<const UNpcDogStateSettings*>(NpcState);
	DogCharacter->SetCurrentAction(DogState->DogAction);
}
