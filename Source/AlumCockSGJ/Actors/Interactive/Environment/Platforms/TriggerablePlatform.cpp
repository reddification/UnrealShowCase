#include "TriggerablePlatform.h"

void ATriggerablePlatform::OnPlatformReachedFinalPosition()
{
	Super::OnPlatformReachedFinalPosition();
	bReverse = !bReverse;	
	PlatformTimeline.Stop();
}
