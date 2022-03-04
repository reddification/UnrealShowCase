#include "AIBaseActivityComponent.h"

#include "AI/Controllers/AIBaseController.h"

void UAIBaseActivityComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerController = Cast<AAIBaseController>(GetOwner());
	OwnerController->ControllerPawnResetEvent.AddUObject(this, &UAIBaseActivityComponent::InitializeActivity);	
}