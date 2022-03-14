#include "BasePlatform.h"

#include "PlatformInvocator.h"
#include "Components/BoxComponent.h"
#include "Data/EnvironmentDamage.h"
#include "GameFramework/Character.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform mesh"));
	PlatformMesh->SetupAttachment(RootComponent);

	BottomCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Bottom collision"));
	BottomCollision->SetupAttachment(PlatformMesh);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();
	StartLocation = GetActorLocation();
	EndLocation = GetActorLocation() + GetActorRotation().RotateVector(EndLocation);
	BottomCollision->OnComponentBeginOverlap.AddDynamic(this, &ABasePlatform::OnBottomCollisionOverlapped);
	
	if (IsValid(PlatformTimelineCurve))
	{
		FOnTimelineFloatStatic PlatformTimelineCallback;
		PlatformTimelineCallback.BindUObject(this, &ABasePlatform::SetPlatformPosition);
		PlatformTimeline.AddInterpFloat(PlatformTimelineCurve, PlatformTimelineCallback);

		FOnTimelineEventStatic OnTimelineEvent;
		OnTimelineEvent.BindUObject(this, &ABasePlatform::OnPlatformReachedFinalPosition);
		PlatformTimeline.SetTimelineFinishedFunc(OnTimelineEvent);
		
		if (IsValid(PlatformInvocator) && PlatformBehavior == EPlatformBehavior::ActivatedByGenerator)
		{
			PlatformInvocator->OnInvocatorActivated.AddDynamic(this, &ABasePlatform::OnPlatformInvoked);
		}

		if (bActive)
        {
            PlatformTimeline.Play();
        }
	}
}

void ABasePlatform::OnPlatformInvoked()
{
	if (!PlatformTimeline.IsPlaying() || PlatformTimeline.IsReversing())
	{
		bReverse = false;
		PlatformTimeline.Play();
	}
}

void ABasePlatform::Start()
{
	if (!PlatformTimeline.IsPlaying())
	{
		bReverse ? PlatformTimeline.Reverse() : PlatformTimeline.Play();
	}
}

void ABasePlatform::ReverseStop()
{
	bReverse = !bReverse;	
	PlatformTimeline.Stop();
}

// Called every frame
void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (PlatformTimeline.IsPlaying())
	{
		PlatformTimeline.TickTimeline(DeltaTime);
	}
}

void ABasePlatform::ResetPlatform()
{
	bReverse = true;
	GetWorld()->GetTimerManager().ClearTimer(this->PlatformReturnDelayTimer);
	PlatformTimeline.Reverse();
}

void ABasePlatform::ResumePlatform()
{
	bReverse = !bReverse;
	bReverse ? PlatformTimeline.Reverse() : PlatformTimeline.Play();
	GetWorld()->GetTimerManager().ClearTimer( PlatformLoopDelayTimer);
}

void ABasePlatform::SetPlatformPosition(float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(PlatformTargetLocation, true);
}

void ABasePlatform::OnPlatformReachedFinalPosition()
{
	switch (PlatformBehavior)
	{
		case EPlatformBehavior::ActivatedByGenerator:
			if (!bReverse)
				GetWorld()->GetTimerManager().SetTimer(PlatformReturnDelayTimer, this, &ABasePlatform::ResetPlatform, PlatformResetDelay, false);
			break;
		case EPlatformBehavior::Loop:
			GetWorld()->GetTimerManager().SetTimer(PlatformLoopDelayTimer, this, &ABasePlatform::ResumePlatform, PlatformLoopDelay, false);
			break;
	case EPlatformBehavior::OnDemand:
		ReverseStop();
		default:
			break;
	}
}

void ABasePlatform::OnBottomCollisionOverlapped(UPrimitiveComponent* PrimitiveComponent, AActor* Actor,
	UPrimitiveComponent* OtherActorComponent, int OtherBodyIndex, bool bFromSweep, const FHitResult& HitResult)
{
	if (HitResult.Normal != FVector::ZeroVector)
		return;
	
	auto EnvironmentDamage = GetEnvironmentDamageEvent();
	PlatformTimeline.Stop();
	Actor->TakeDamage(DamageOnCollision, EnvironmentDamage, nullptr, this);
	OnPlatformReachedFinalPosition();
}
