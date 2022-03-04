#include "InteractiveActor.h"

#include "Characters/BaseHumanoidCharacter.h"

// Called when the game starts or when spawned
void AInteractiveActor::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractionVolume))
	{
		InteractionVolume->OnComponentBeginOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapBegin);
		InteractionVolume->OnComponentEndOverlap.AddDynamic(this, &AInteractiveActor::OnInteractionVolumeOverlapEnd);
	}
}

void AInteractiveActor::OnInteractionVolumeOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseHumanoidCharacter* Character = Cast<ABaseHumanoidCharacter>(OtherActor);
	if (!IsOverlappingCharacterCapsule(Character, OtherComp))
		return;

	Character->RegisterInteractiveActor(this);
}

void AInteractiveActor::OnInteractionVolumeOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseHumanoidCharacter* Character = Cast<ABaseHumanoidCharacter>(OtherActor);
	if (!IsOverlappingCharacterCapsule(Character, OtherComp))
		return;

	Character->UnregisterInteractiveActor(this);
}

bool AInteractiveActor::IsOverlappingCharacterCapsule(const ACharacter* Character, const UPrimitiveComponent* OtherComp) const
{
	return IsValid(Character) && OtherComp == reinterpret_cast<UPrimitiveComponent*>(Character->GetCapsuleComponent());
}
