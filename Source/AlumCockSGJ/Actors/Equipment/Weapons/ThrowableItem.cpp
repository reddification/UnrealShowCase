#include "Actors/Equipment/Weapons/ThrowableItem.h"

#include "DrawDebugHelpers.h"
#include "Actors/Projectiles/Projectile.h"
#include "Actors/Projectiles/ThrowableProjectile.h"
#include "Characters/BaseCharacter.h"
#include "Components/Character/CharacterEquipmentComponent.h"

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	checkf(IsValid(ItemSettings) && ItemSettings->IsA<UThrowableItemSettings>(), TEXT("Item settings must be a valid ThrowableItemSettings DataAsset"))
	ThrowableItemSettings = StaticCast<const UThrowableItemSettings*>(ItemSettings);
	SpawnAttachedProjectile();
}

bool AThrowableItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpData)
{
	bool bAdded = Super::TryAddToEquipment(EquipmentComponent, PickUpData);
	if (!bAdded)
		return false;

	EquipmentComponent->EquipThrowable(ThrowableItemSettings->ThrowableSlot);
	return true;
}

bool AThrowableItem::TryAddToLoadout(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpSettings)
{
	auto EquippedThrowable = EquipmentComponent->Throwables[(uint32)ThrowableItemSettings->ThrowableSlot];
	if (IsValid(EquippedThrowable))
		EquippedThrowable->Destroy();
	
	EquipmentComponent->Throwables[(uint32)ThrowableItemSettings->ThrowableSlot] = this;
	return true;
}

void AThrowableItem::Destroyed()
{
	if (AttachedProjectile.IsValid())
		AttachedProjectile->Destroy();

	Super::Destroyed();
}

void AThrowableItem::Throw(AController* OwnerController)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	OwnerController->GetPlayerViewPoint(ViewPoint, ViewRotation);
	FVector LaunchDirection = ViewRotation.Vector();

	CurrentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	FHitResult TraceResult;
	const FVector TraceEnd = ViewPoint + LaunchDirection * ThrowableItemSettings->ThrowSpeed;
	bool bHit = GetWorld()->LineTraceSingleByChannel(TraceResult, ViewPoint, TraceEnd, ECC_Visibility);
	LaunchDirection = bHit || TraceResult.bBlockingHit
		? (TraceResult.ImpactPoint - CurrentProjectile->GetActorLocation()).GetSafeNormal()
		: (TraceEnd - CurrentProjectile->GetActorLocation()).GetSafeNormal();

	FVector ViewUpVector = ViewRotation.RotateVector(FVector::UpVector);
	LaunchDirection = LaunchDirection + FMath::Tan(FMath::DegreesToRadians(ThrowableItemSettings->ThrowAngle)) * ViewUpVector;
	
	CurrentProjectile->LaunchProjectile(LaunchDirection.GetSafeNormal(),
		GetOwner()->GetVelocity().Size() + ThrowableItemSettings->ThrowSpeed, OwnerController);
}

void AThrowableItem::Activate(AController* Controller)
{
	if (CurrentProjectile.IsValid())
	{
		CurrentProjectile->Activate(Controller);
	}
}

ABaseThrowableProjectile* AThrowableItem::GetCurrentProjectile()
{
	return CurrentProjectile.Get(); 
}

ABaseThrowableProjectile* AThrowableItem::SpawnProjectile()
{
	CurrentProjectile = GetWorld()->SpawnActor<ABaseThrowableProjectile>(ThrowableItemSettings->ProjectileClass);
	CurrentProjectile->SetOwner(GetOwner());
	return CurrentProjectile.Get();
}

void AThrowableItem::DropProjectile(AController* Controller)
{
	if (CurrentProjectile.IsValid())
	{
		CurrentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentProjectile->Drop(Controller);
	}
}

void AThrowableItem::SpawnAttachedProjectile()
{
	AttachedProjectile = GetWorld()->SpawnActor<ABaseThrowableProjectile>(ThrowableItemSettings->ProjectileClass, GetActorLocation(), FRotator::ZeroRotator);
	AttachedProjectile->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	AttachedProjectile->SetOwner(GetOwner());
}

void AThrowableItem::DestroyAttachedProjectile()
{
	AttachedProjectile->Destroy();
	AttachedProjectile.Reset();
}

const UThrowableItemSettings* AThrowableItem::GetThrowableItemSettings() const
{
	if (ThrowableItemSettings.IsValid())
		return ThrowableItemSettings.Get();

	checkf(IsValid(ItemSettings) && ItemSettings->IsA<UThrowableItemSettings>(), TEXT("Item settings must be a valid ThrowableItemSettings DataAsset"))
	return StaticCast<const UThrowableItemSettings*>(ItemSettings);
}

bool AThrowableItem::CanBePickedUp(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	// auto AmmoType = GetAmmoType();
	// bool bCanResupply = CharacterEquipmentComponent->EquipmentSettings->AmmunitionLimits[AmmoType] > CharacterEquipmentComponent->Pouch[(uint8)AmmoType];
	// return bCanResupply && !IsValid(CharacterEquipmentComponent->Throwables[(uint8)ItemSettings->DesignatedSlot]);
	auto ThrowableSettings = GetThrowableItemSettings();
	return !IsValid(CharacterEquipmentComponent->Throwables[(uint8)ThrowableSettings->ThrowableSlot]);
}
