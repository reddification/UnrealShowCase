#include "Puddle.h"

#include "Characters/Interfaces/Soakable.h"

APuddle::APuddle()
{
	PuddleMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PuddleMesh"));
	PuddleMeshComponent->SetupAttachment(RootComponent);
}

void APuddle::BeginPlay()
{
	Super::BeginPlay();
	PuddleMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &APuddle::OnPuddleSteppedInto);
	PuddleMeshComponent->OnComponentEndOverlap.AddDynamic(this, &APuddle::OnPuddleSteppedOut);
}

void APuddle::OnPuddleSteppedInto(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Soakable = Cast<ISoakable>(OtherActor);
	if (!Soakable)
		return;

	Soakable->StartSoaking();
}

void APuddle::OnPuddleSteppedOut(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto Soakable = Cast<ISoakable>(OtherActor);
	if (!Soakable)
		return;

	Soakable->StopSoaking();
}