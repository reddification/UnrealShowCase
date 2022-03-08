#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Combat/MeleeHitRegistratorComponent.h"

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistratorComponent>(HitRegistrators);
	for (const auto HitRegistrator : HitRegistrators)
	{
		HitRegistrator->MeleeHitRegisteredEvent.BindUObject(this, &AMeleeWeaponItem::OnMeleeHitRegistered);
	}
}

void AMeleeWeaponItem::OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	Super::OnEquipped(CharacterEquipmentComponent);
	CharacterEquipmentComponent->EquippedRangedWeapon.Reset();
	CharacterEquipmentComponent->EquippedMeleeWeapon = this;
	CharacterEquipmentComponent->AimingSpeedResetEvent.ExecuteIfBound();
	CharacterEquipmentComponent->MeleeWeaponEquippedEvent.ExecuteIfBound();
}

void AMeleeWeaponItem::OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
	Super::OnUnequipped(CharacterEquipmentComponent);
	CharacterEquipmentComponent->EquippedMeleeWeapon.Reset();
}

bool AMeleeWeaponItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent,
	const FPickUpItemData& PickUpItemData)
{
	auto DesignatedSlot = ItemSettings->DesignatedSlot;
	auto ExistingItem = EquipmentComponent->Loadout[(uint8)DesignatedSlot];
	if (IsValid(ExistingItem))
		EquipmentComponent->DropWeapon(ItemSettings->DesignatedSlot, PickUpItemData.PickUpLocation);

	EquipmentComponent->Loadout[(uint8)DesignatedSlot] = this;
	return Super::TryAddToEquipment(EquipmentComponent, PickUpItemData);
}

void AMeleeWeaponItem::SetIsHitRegistrationEnabled(bool bEnabled)
{
	HitActors.Empty();
	for (auto HitRegistrator : HitRegistrators)
	{
		HitRegistrator->SetIsEnabled(bEnabled);
	}
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackType AttackType, AController* Controller)
{
	HitActors.Empty();
	ActiveAttack = Attacks.Find(AttackType);
	AttackerController = Controller;
}

void AMeleeWeaponItem::StopAttack()
{
	SetIsHitRegistrationEnabled(false);	// idk
}

void AMeleeWeaponItem::OnMeleeHitRegistered(const FHitResult& HitResult, const FVector& Direction)
{
	AActor* HitActor = HitResult.GetActor();
	if (!IsValid(HitActor) || HitActors.Contains(HitActor) || !ActiveAttack)
	{
		return;
	}

	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = Direction;
	DamageEvent.DamageTypeClass = ActiveAttack->DamageTypeClass;
	HitActor->TakeDamage(ActiveAttack->DamageAmount, DamageEvent, AttackerController, GetOwner());
	HitActors.Add(HitActor);
}
