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
}

void AThrowableItem::Destroyed()
{
	if (IsValid(AttachedProjectile))
		AttachedProjectile->Destroy();

	Super::Destroyed();
}

void AThrowableItem::Throw(AController* OwnerController)
{
	FVector ViewPoint;
	FRotator ViewRotation;
	OwnerController->GetPlayerViewPoint(ViewPoint, ViewRotation);
	FVector LaunchDirection = ViewRotation.Vector();

	if (!IsValid(CurrentProjectile))
		return;
	
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
	if (IsValid(CurrentProjectile))
	{
		CurrentProjectile->Activate(Controller);
	}
}

ABaseThrowableProjectile* AThrowableItem::GetCurrentProjectile()
{
	return CurrentProjectile; 
}

ABaseThrowableProjectile* AThrowableItem::SpawnProjectile()
{
	CurrentProjectile = GetWorld()->SpawnActor<ABaseThrowableProjectile>(ThrowableItemSettings->ProjectileClass);
	CurrentProjectile->SetOwner(GetOwner());
	return CurrentProjectile;
}

void AThrowableItem::DropProjectile(AController* Controller)
{
	if (IsValid(CurrentProjectile))
	{
		CurrentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentProjectile->Drop(Controller);
	}
}

void AThrowableItem::SpawnAttachedProjectile()
{
	AttachedProjectile = GetWorld()->SpawnActor<ABaseThrowableProjectile>(ThrowableItemSettings->ProjectileClass, GetActorLocation(), FRotator::ZeroRotator);
	AttachedProjectile->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	AttachedProjectile->SetOwner(GetOwner());
}

void AThrowableItem::DestroyAttachedProjectile()
{
	if (IsValid(AttachedProjectile))
		AttachedProjectile->Destroy();
}

const UThrowableItemSettings* AThrowableItem::GetThrowableItemSettings() const
{
	if (ThrowableItemSettings)
		return ThrowableItemSettings;

	checkf(IsValid(ItemSettings) && ItemSettings->IsA<UThrowableItemSettings>(), TEXT("Item settings must be a valid ThrowableItemSettings DataAsset"))
	return StaticCast<const UThrowableItemSettings*>(ItemSettings);
}

bool AThrowableItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpData)
{
	auto AmmoType = GetAmmoType();
	int32 AmmoLimit = EquipmentComponent->GetAmmunationLimit(AmmoType);
	if (AmmoLimit <= 0)
		return false;
	
	auto Settings = GetThrowableItemSettings();
	auto ThrowableSlot = Settings->ThrowableSlot;
	auto ThrowableInSlot = EquipmentComponent->Throwables[(uint8)ThrowableSlot];
	
	if (IsValid(ThrowableInSlot))
	{
		if (AmmoType != ThrowableInSlot->GetAmmoType())
			EquipmentComponent->DropThrowable(ThrowableSlot, PickUpData.PickUpLocation);
	}
	
	bool bAdded = Super::TryAddToEquipment(EquipmentComponent, PickUpData);
	if (!bAdded)
		return false;

	if (EquipmentComponent->EquippedThrowableSlot == EThrowableSlot::None || EquipmentComponent->EquippedThrowableSlot == ThrowableSlot)
		EquipmentComponent->EquipThrowable(Settings->ThrowableSlot);

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


void AThrowableItem::OnDropped(UCharacterEquipmentComponent* EquipmentComponent,
	APickableEquipmentItem* PickableEquipmentItem)
{
	int32& Ammo = EquipmentComponent->Pouch[(uint8)GetAmmoType()];
	PickableEquipmentItem->SetDroppedState(Ammo);
	Ammo = 0;
}

bool AThrowableItem::CanBePickedUp(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	auto ThrowableSettings = GetThrowableItemSettings();
	auto ThrowableInSlot = CharacterEquipmentComponent->Throwables[(uint8)ThrowableSettings->ThrowableSlot];
	return !IsValid(ThrowableInSlot)
		|| CharacterEquipmentComponent->Pouch[(uint8)ThrowableInSlot->GetAmmoType()] <= 0;
}
