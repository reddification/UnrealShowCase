#include "AIBasicActivityComponent.h"

#include "AI/Controllers/AIScissorsController.h"

void UAIBasicActivityComponent::BeginPlay()
{
	Super::BeginPlay();
	InitializeActivity();
}

void UAIBasicActivityComponent::InitializeActivity()
{
	Super::InitializeActivity();
	auto BasicActivityCharacter = Cast<IBasicActivityCharacter>(OwnerController->GetPawn());
	if (BasicActivityCharacter)
		OwnerController->RunBehaviorTree(BasicActivityCharacter->GetDefaultBehaviorTree());
}
