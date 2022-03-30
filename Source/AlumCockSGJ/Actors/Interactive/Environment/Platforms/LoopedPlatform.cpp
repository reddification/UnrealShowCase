#include "LoopedPlatform.h"

void ALoopedPlatform::BeginPlay()
{
	bActive = true;	
	Super::BeginPlay();
}

void ALoopedPlatform::ResumePlatform()
{
	bReverse = !bReverse;
	bReverse ? PlatformTimeline.Reverse() : PlatformTimeline.Play();
}

void ALoopedPlatform::OnPlatformReachedFinalPosition()
{
	Super::OnPlatformReachedFinalPosition();
	GetWorld()->GetTimerManager().SetTimer(DelayTimer, this, &ALoopedPlatform::ResumePlatform, PlatformLoopDelay, false);
}
