#include "InvokablePlatform.h"

#include "PlatformInvocator.h"

void AInvokablePlatform::BeginPlay()
{
	Super::BeginPlay();
	if (PlatformInvocator)
		PlatformInvocator->OnInvocatorActivated.AddUObject(this, &AInvokablePlatform::OnPlatformInvoked);
}

void AInvokablePlatform::OnPlatformReachedFinalPosition()
{
	Super::OnPlatformReachedFinalPosition();
	if (!bWaitUntilInvokerReleased && GetActorLocation() != StartLocation)
		GetWorld()->GetTimerManager().SetTimer(PlatformReturnDelayTimer, this, &AInvokablePlatform::ReturnPlatform, ReleasePlatformAfter, false);
}

void AInvokablePlatform::ReturnPlatform()
{
	bReverse = true;
	PlatformTimeline.Reverse();
}

void AInvokablePlatform::OnPlatformInvoked(bool bInvoked)
{
	if (bInvoked)
	{
		if (!PlatformTimeline.IsPlaying() || PlatformTimeline.IsReversing())
		{
			bReverse = false;
			PlatformTimeline.Play();
		}
	}
	else if (bWaitUntilInvokerReleased)
	{
		if (GetActorLocation() != StartLocation)
		{
			PlatformTimeline.Reverse();
			bReverse = true;
		}
	}
}


