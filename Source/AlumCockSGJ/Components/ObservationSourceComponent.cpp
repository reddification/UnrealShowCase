#include "ObservationSourceComponent.h"
#include "DrawDebugHelpers.h"
#include "Characters/PlayerCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Data/DataAssets/Components/ObservationSourceSettings.h"
#include "Kismet/GameplayStatics.h"

void UObservationSourceComponent::BeginPlay()
{
	Super::BeginPlay();
	auto PlayerController = Cast<ABasePlayerController>(GetWorld()->GetFirstPlayerController());
	if (!IsValid(PlayerController))
		return;

	PlayerController->RegisterObservationSource(this);
	// bullshit isn't it
	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	ObservationSquaredDistanceThreshold = ObservationSourceSettings->ObservationDistanceThreshold * ObservationSourceSettings->ObservationDistanceThreshold;
	bSightSourceActive = true;
	bInSight = false;
}

void UObservationSourceComponent::UpdateVisibility()
{
	if (!PlayerCharacter || !ObservationSourceSettings)
		return;
	
	FVector CharacterVisionComponentLocation = PlayerCharacter->GetCharacterVisionComponent()->GetComponentLocation();
	FVector ComponentLocation = GetComponentLocation();

	bool bIsObservableInSight = false;
	if (FVector::DistSquared(CharacterVisionComponentLocation, ComponentLocation) <= ObservationSquaredDistanceThreshold)
	{
		float dp = FVector::DotProduct(PlayerCharacter->GetViewRotation().Vector(), (ComponentLocation - CharacterVisionComponentLocation).GetSafeNormal());
		if (dp >= ObservationSourceSettings->PlayerViewDirectionDotProductThreshold)
		{
			FHitResult HitResult;
			FCollisionQueryParams CollisionQueryParams;
			CollisionQueryParams.AddIgnoredActor(GetOwner());
			bIsObservableInSight = !GetWorld()->LineTraceSingleByChannel(HitResult, ComponentLocation, CharacterVisionComponentLocation,
				ECC_Visibility, CollisionQueryParams);

			#if !UE_BUILD_SHIPPING
			if (ObservationSourceSettings->bDrawDebug)
				DrawDebugLine(GetWorld(), ComponentLocation, CharacterVisionComponentLocation, FColor::Red, false, 5);
			#endif
		}
	}

	if (bIsObservableInSight == bInSight)
		return;

	bInSight = bIsObservableInSight;
	SightStateChangedEvent.Broadcast(bInSight);
}
