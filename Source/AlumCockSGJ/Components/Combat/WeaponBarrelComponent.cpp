#include "WeaponBarrelComponent.h"
#include "Data/DataAssets/Items/BarrelSettings.h"
#include "Data/DataAssets/Items/WeaponBarrelSettings.h"

void UWeaponBarrelComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(BarrelSettings->IsA<UWeaponBarrelSettings>(), TEXT("Barrel settings must be a valid UWeaponBarrelSettings DataAsset"))
	WeaponBarrelSettings = StaticCast<const UWeaponBarrelSettings*>(BarrelSettings);
}

AActor* UWeaponBarrelComponent::GetDamagingActor() const
{
	AActor* DamagingActor = GetOwner()->GetOwner();
	return IsValid(DamagingActor) ? DamagingActor : Super::GetDamagingActor();
}

const UWeaponBarrelSettings* UWeaponBarrelComponent::GetWeaponBarrelSettings() const
{
	if (WeaponBarrelSettings.IsValid())
		return WeaponBarrelSettings.Get();
	
	checkf(BarrelSettings->IsA<UWeaponBarrelSettings>(), TEXT("Barrel settings must be a valid UWeaponBarrelSettings DataAsset"))
	return StaticCast<const UWeaponBarrelSettings*>(BarrelSettings);
}
