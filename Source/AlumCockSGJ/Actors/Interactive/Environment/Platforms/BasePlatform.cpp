#include "BasePlatform.h"

#include "Components/BoxComponent.h"
#include "Data/EnvironmentDamage.h"
#include "GameFramework/Character.h"

ABasePlatform::ABasePlatform()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	NetUpdateFrequency = 50.f;
	SetReplicateMovement(true);
	
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
	
	FOnTimelineFloatStatic PlatformTimelineCallback;
	PlatformTimelineCallback.BindUObject(this, &ABasePlatform::SetPlatformPosition);
	PlatformTimeline.AddInterpFloat(PlatformTimelineCurve, PlatformTimelineCallback);

	FOnTimelineEventStatic OnTimelineEvent;
	OnTimelineEvent.BindUObject(this, &ABasePlatform::OnPlatformReachedFinalPosition);
	PlatformTimeline.SetTimelineFinishedFunc(OnTimelineEvent);

	if (bActive)
        PlatformTimeline.Play();
}

void ABasePlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority && PlatformTimeline.IsPlaying())
		PlatformTimeline.TickTimeline(DeltaTime);
}

void ABasePlatform::Start()
{
	if (!PlatformTimeline.IsPlaying())
	{
		bReverse ? PlatformTimeline.Reverse() : PlatformTimeline.Play();
	}
}

void ABasePlatform::SetPlatformPosition(float Alpha)
{
	const FVector PlatformTargetLocation = FMath::Lerp(StartLocation, EndLocation, Alpha);
	SetActorLocation(PlatformTargetLocation, true);
}

void ABasePlatform::OnPlatformReachedFinalPosition()
{
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
