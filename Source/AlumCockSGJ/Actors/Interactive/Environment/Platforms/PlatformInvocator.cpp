#include "PlatformInvocator.h"

#include "CommonConstants.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

APlatformInvocator::APlatformInvocator()
{
	bReplicates = true;
	NetUpdateFrequency = 20.f;
	
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("root"));
	RootComponent = DefaultPlatformRoot;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Volume"));
	TriggerBox->SetCollisionProfileName(ProfileInteractionVolume);
	TriggerBox->SetupAttachment(RootComponent);
}

void APlatformInvocator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlatformInvocator, bIsActivated)
}

void APlatformInvocator::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformInvocator::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformInvocator::OnTriggerOverlapEnd);
}

void APlatformInvocator::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* OtherPawn = Cast<APawn>(OtherActor);
	if (!IsValid(OtherPawn))
		return;

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.AddUnique(OtherPawn);
		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			OnIsActivated(true);
		}	
	}
}

void APlatformInvocator::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APawn* OtherPawn = Cast<APawn>(OtherActor);
	if (!IsValid(OtherPawn))
		return;

	if (OtherPawn->IsLocallyControlled() || GetLocalRole() == ROLE_Authority)
	{
		OverlappedPawns.RemoveSingleSwap(OtherPawn);
		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			bIsActivated = false;
			OnIsActivated(false);
		}
	}
}

void APlatformInvocator::OnIsActivated(bool bIsActivated_In)
{
	// bIsActivated = bIsActivated_In;
	if (OnInvocatorActivated.IsBound())
		OnInvocatorActivated.Broadcast(bIsActivated);
}

void APlatformInvocator::OnRep_IsActivated(bool bIsActivatedPrevious)
{
	// don't need this check since DOREPLIFETIME default params makes this method to be called only when the property value actually changed
	// if (bIsActivated != bIsActivatedPrevious)
	OnIsActivated(bIsActivated);
}

void APlatformInvocator::Multicast_SetIsActivated_Implementation(bool bIsActivated_In)
{
	bIsActivated = bIsActivated_In;
	if (OnInvocatorActivated.IsBound())
		OnInvocatorActivated.Broadcast(bIsActivated);
}

bool APlatformInvocator::Multicast_SetIsActivated_Validate(bool bIsActivated_In)
{
	return true;
}
