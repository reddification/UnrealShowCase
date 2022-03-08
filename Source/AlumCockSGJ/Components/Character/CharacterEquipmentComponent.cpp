#include "Components/Character/CharacterEquipmentComponent.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Weapons/ThrowableItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Actors/Interactive/Pickables/PickableEquipmentItem.h"
#include "Actors/Projectiles/Projectile.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	const auto Owner = GetOwner();
	checkf(Owner->IsA<ABaseCharacter>(), TEXT("CharacterEquipmentComponent is supposed to work only with AGCBaseCharacter derivatives"));
	checkf(IsValid(EquipmentSettings), TEXT("Equipment settings must be set!"));
	CharacterOwner = StaticCast<ABaseCharacter*>(Owner);
	EquippedSlot = EEquipmentSlot::None;
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	Loadout.AddZeroed((uint32)EEquipmentSlot::MAX);
	Throwables.AddZeroed((uint32)EThrowableSlot::MAX);
	Pouch.AddZeroed((uint32)EAmmunitionType::MAX);

	FPickUpItemData PickUpItemData;
	PickUpItemData.bForce = false;
	PickUpItemData.bCreatingLoadout = true;
	PickUpItemData.bDropped = false;
	PickUpItemData.PickUpLocation = GetOwner()->GetActorLocation();
	
	for (const auto& LoadoutSlot : EquipmentSettings->InitialLoadout)
	{
	    if (IsValid(LoadoutSlot.EquippableItemClass))
	    {
	    	auto DefaultObject = LoadoutSlot.EquippableItemClass.GetDefaultObject();
	        PickUpItem(LoadoutSlot.EquippableItemClass, PickUpItemData);
	        PickUpAmmo(DefaultObject->GetAmmoType(), LoadoutSlot.InitialSupply, DefaultObject->IsA<AThrowableItem>());
	    }
	}

	EquipItem(EquipmentSettings->InitialEquippedWeapon);
	EquipThrowable(EquipmentSettings->InitialEquippedThrowable);
}

void UCharacterEquipmentComponent::EquipPrimaryWeapon()
{
	if (EquippedSlot != EEquipmentSlot::PrimaryWeapon)
		EquipItem(EEquipmentSlot::PrimaryWeapon);
}

void UCharacterEquipmentComponent::EquipSecondaryWeapon()
{
	if (EquippedSlot != EEquipmentSlot::SideArm)
		EquipItem(EEquipmentSlot::SideArm);
}

void UCharacterEquipmentComponent::EquipMeleeWeapon()
{
	if (EquippedSlot != EEquipmentSlot::MeleeWeapon)
		EquipItem(EEquipmentSlot::MeleeWeapon);
}

void UCharacterEquipmentComponent::EquipPrimaryThrowable()
{
	if (EquippedThrowableSlot != EThrowableSlot::Explosive)
		EquipThrowable(EThrowableSlot::Explosive);
}

void UCharacterEquipmentComponent::EquipSecondaryThrowable()
{
	if (EquippedThrowableSlot != EThrowableSlot::Secondary)
		EquipThrowable(EThrowableSlot::Secondary);
}

bool UCharacterEquipmentComponent::IsAnythingEquipped() const
{
	return (ActiveThrowable.IsValid() && Pouch[(uint8)ActiveThrowable->GetAmmoType()] > 0) || EquippedRangedWeapon.IsValid() || EquippedMeleeWeapon.IsValid();
}

bool UCharacterEquipmentComponent::PickUpItem(const TSubclassOf<AEquippableItem>& ItemClass, const FPickUpItemData& PickUpItemData)
{
	auto DefaultItem = ItemClass.GetDefaultObject();
	if (!PickUpItemData.bForce && !DefaultItem->CanBePickedUp(this))
		return false;

	AEquippableItem* Item = GetWorld()->SpawnActor<AEquippableItem>(ItemClass);
    return Item->TryAddToEquipment(this, PickUpItemData);
}

bool UCharacterEquipmentComponent::PickUpAmmo(EAmmunitionType AmmoType, int Ammo, bool bThrowable)
{
	if (Ammo <= 0)
		return false;
	
	if (bThrowable && ActiveThrowable.IsValid() && ActiveThrowable->GetAmmoType() != AmmoType)
		return false;
	
	int32 AmmoLimit = GetAmmunationLimit(AmmoType);
	if (AmmoLimit <= 0)
		return false;
	
	int32& CurrentAmmo = Pouch[(uint8)AmmoType];
	int32 NewAmmo = FMath::Min(AmmoLimit, CurrentAmmo + Ammo);
	bool bPickedUp = NewAmmo != CurrentAmmo;
	
	if (ActiveThrowable.IsValid() && ActiveThrowable->GetAmmoType() == AmmoType && bPickedUp)
	{
		if (CurrentAmmo <= 0)
			ActiveThrowable->SpawnAttachedProjectile();

		CurrentAmmo = NewAmmo;
		ThrowablesCountChanged.ExecuteIfBound(NewAmmo);
	}
	
	if (EquippedRangedWeapon.IsValid() && EquippedRangedWeapon->GetAmmunitionType() == AmmoType)
		WeaponAmmoChangedEvent.ExecuteIfBound(EquippedRangedWeapon->GetAmmo(), NewAmmo);
	
	CurrentAmmo = NewAmmo;
	return bPickedUp;
}

#pragma region CHANGING EQUIPMENT

void UCharacterEquipmentComponent::EquipItem(int delta)
{
	if (bChangingEquipment || delta < 0 && EquippedSlot == EEquipmentSlot::None)
		return;
	
	uint8 CurrentSlotIndex = (uint8)EquippedSlot;
	uint8 NextSlotIndex = CurrentSlotIndex + delta;
	while (NextSlotIndex > (uint8)EEquipmentSlot::None && NextSlotIndex < (uint8)EEquipmentSlot::MAX && !IsValid(Loadout[NextSlotIndex]))
		NextSlotIndex += delta;

	if (NextSlotIndex == (uint8)EEquipmentSlot::None)
		UnequipItem();
	else if (NextSlotIndex != (uint8)EEquipmentSlot::MAX)
		EquipItem((EEquipmentSlot)NextSlotIndex);
}

// TODO perhaps equipping a weapon should be done in 2 phases: first unequip current weapon and only then start another timer to equip another weapon;
void UCharacterEquipmentComponent::EquipItem(EEquipmentSlot NewSlot)
{
	if (bChangingEquipment || !IsValid(Loadout[(uint32)NewSlot]) || !CharacterOwner->CanStartAction(ECharacterAction::ChangeEquipment))
		return;
	
	const auto NewWeapon = Loadout[(uint32)NewSlot];
	if (IsValid(NewWeapon))
	{
		const float EquipmentDuration = NewWeapon->GetEquipmentDuration() * EquipmentSettings->EquipDurationMultiplier;
		bChangingEquipment = true;

		ActiveEquippingData.OldItem = GetEquippedItem();
		ActiveEquippingData.NewItem = NewWeapon;

		ActiveEquippingData.Montage = NewWeapon->GetCharacterEquipMontage();
		ActiveEquippingData.EquipmentSlot = NewSlot;
		ActiveEquippingData.bNotified = false;

		if (IsValid(ActiveEquippingData.Montage))
			CharacterOwner->PlayAnimMontageWithDuration(ActiveEquippingData.Montage, EquipmentDuration);
		
		GetWorld()->GetTimerManager().SetTimer(ChangingEquipmentTimer, this, &UCharacterEquipmentComponent::OnWeaponsChanged, EquipmentDuration);
		CharacterOwner->OnActionStarted(ECharacterAction::ChangeEquipment);
	}
}

void UCharacterEquipmentComponent::EquipThrowable(EThrowableSlot ThrowableType)
{
	AThrowableItem* Throwable = Throwables[(uint32)ThrowableType];
	if (IsValid(Throwable))
	{
		int32 Count = Pouch[(uint32)Throwable->GetAmmoType()];
		EquippedThrowableSlot = ThrowableType;
		ActiveThrowable = Throwable;
		Throwable->AttachToComponent(CharacterOwner->GetMesh(),  FAttachmentTransformRules::KeepRelativeTransform,
			Throwable->GetCharacterUnequippedSocketName());

		if (ThrowableEquippedEvent.IsBound())
		{
			ThrowableEquippedEvent.ExecuteIfBound(Throwable, Count);
		}
	}
}

void UCharacterEquipmentComponent::OnWeaponsChanged()
{
	if (IsValid(ActiveEquippingData.OldItem))
	{
		// WeaponUnequippedEvent.Broadcast();
		ActiveEquippingData.OldItem->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform,
			ActiveEquippingData.OldItem->GetCharacterUnequippedSocketName());
		ActiveEquippingData.OldItem->OnUnequipped(this);
	}

	bool bNewWeaponValid = IsValid(ActiveEquippingData.NewItem);
	if (bNewWeaponValid)
		ActiveEquippingData.NewItem->OnEquipped(this);
	
	ActiveEquippingData.bNotified = true;
	
	PreviousEquippedSlot = EquippedSlot;
	EquippedSlot = ActiveEquippingData.EquipmentSlot;
	CharacterOwner->UpdateStrafingControls();
	bChangingEquipment = false;
	
	if (bNewWeaponValid)
	{
		if (WeaponEquippedEvent.IsBound())
			WeaponEquippedEvent.Broadcast(ActiveEquippingData.NewItem->GetName(), ActiveEquippingData.NewItem->GetReticleType());
	}
	else if (WeaponUnequippedEvent.IsBound())
	{
		WeaponUnequippedEvent.Broadcast();
	}
		
	CharacterOwner->OnActionEnded(ECharacterAction::ChangeEquipment);
}

void UCharacterEquipmentComponent::InterruptChangingEquipment()
{
	if (!bChangingEquipment)
	{
		CharacterOwner->OnActionEnded(ECharacterAction::ChangeEquipment);
		return;
	}

	bool bClearTimer = true;
	if (IsValid(ActiveEquippingData.Montage))
	{
		if (ActiveEquippingData.bNotified)
		{
			bClearTimer = false;
		}
	}

	if (bClearTimer)
	{
		GetWorld()->GetTimerManager().ClearTimer(ChangingEquipmentTimer);
	}
	
	CharacterOwner->OnActionEnded(ECharacterAction::ChangeEquipment);
	bChangingEquipment = false;
}

void UCharacterEquipmentComponent::UnequipItem()
{
	if (EquippedSlot == EEquipmentSlot::None || bChangingEquipment || !CharacterOwner->CanStartAction(ECharacterAction::ChangeEquipment))
		return;

	ActiveEquippingData.OldItem = GetEquippedItem();
	const float EquipmentDuration = GetEquippedItem()->GetUnequipDuration() * EquipmentSettings->EquipDurationMultiplier;
	bChangingEquipment = true;

	ActiveEquippingData.NewItem = nullptr;

	ActiveEquippingData.Montage = GetEquippedItem()->GetCharacterUnequipMontage();
	ActiveEquippingData.EquipmentSlot = EEquipmentSlot::None;
	ActiveEquippingData.bNotified = false;

	if (IsValid(ActiveEquippingData.Montage))
		CharacterOwner->PlayAnimMontageWithDuration(ActiveEquippingData.Montage, EquipmentDuration);
		
	GetWorld()->GetTimerManager().SetTimer(ChangingEquipmentTimer, this, &UCharacterEquipmentComponent::OnWeaponsChanged, EquipmentDuration);
	CharacterOwner->OnActionStarted(ECharacterAction::ChangeEquipment);
}

void UCharacterEquipmentComponent::DropWeapon(EEquipmentSlot Slot, const FVector& WorldItemSpawnLocation)
{
	auto Item = Loadout[(uint8)Slot];
	if (!IsValid(Item))
		return;

	if (bReloading)
		InterruptReloading();
		
	auto WeaponDTR = Item->GetWeaponDTR();
	if (WeaponDTR)
	{
		FActorSpawnParameters ActorSpawnParameters;
		ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		auto SpawnedItem = GetWorld()->SpawnActor<APickableEquipmentItem>(WeaponDTR->SpawnWorldItemClass, WorldItemSpawnLocation, FRotator::ZeroRotator,
			ActorSpawnParameters);
		SpawnedItem->SetItemDTRH(Item->GetWeaponDTRH());
		Item->OnDropped(this, SpawnedItem);
	}

	Item->Destroy();
}

void UCharacterEquipmentComponent::DestroyEquipment()
{
	for (auto LoadoutItem: Loadout)
	{
		if (IsValid(LoadoutItem))
			LoadoutItem->Destroy();
	}

	for (auto ThrowableItem : Throwables)
	{
		if (IsValid(ThrowableItem))
			ThrowableItem->Destroy();
	}
}

#pragma endregion CHANGING EQUIPMENT

#pragma region RELOAD

bool UCharacterEquipmentComponent::CanReload()
{
	if (bReloading || !EquippedRangedWeapon.IsValid() || !CharacterOwner->CanStartAction(ECharacterAction::Reload))
	{
		return false;
	}

	int32 ClipCapacity = EquippedRangedWeapon->GetClipCapacity();
	int32 CurrentAmmo = EquippedRangedWeapon->GetAmmo();
	if (CurrentAmmo >= ClipCapacity)
		return false;

	return !EquippedRangedWeapon->IsInfiniteAmmoSupply()
		? Pouch[(uint32)EquippedRangedWeapon->GetAmmunitionType()] > 0
		: true;
}

void UCharacterEquipmentComponent::TryReload()
{
	if (!CanReload()) return;
	
	bReloading = true;
	CharacterOwner->OnActionStarted(ECharacterAction::Reload);
	const float ReloadDuration = EquippedRangedWeapon->GetReloadDuration() * EquipmentSettings->ReloadDurationMultiplier;

	UAnimMontage* CharacterReloadMontage = EquippedRangedWeapon->GetCharacterReloadMontage();
	if (EquippedRangedWeapon->GetReloadType() != EReloadType::ShellByShell || !IsValid(CharacterReloadMontage))
	{
		GetWorld()->GetTimerManager().SetTimer(ReloadTimer, this, &UCharacterEquipmentComponent::CompleteReloading, ReloadDuration);
	}
	
	EquippedRangedWeapon->StartReloading(ReloadDuration);
	if (IsValid(CharacterReloadMontage))
	{
		CharacterOwner->PlayAnimMontageWithDuration(CharacterReloadMontage, ReloadDuration);
	}
}

void UCharacterEquipmentComponent::InterruptReloading()
{
	if (!bReloading)
	{
		CharacterOwner->OnActionEnded(ECharacterAction::Reload);
		return;
	}
	
	bReloading = false;
	if (EquippedRangedWeapon.IsValid())
	{
		EquippedRangedWeapon->StopReloading(true);
		UAnimMontage* CharacterReloadMontage = EquippedRangedWeapon->GetCharacterReloadMontage();
		if (IsValid(CharacterReloadMontage))
		{
			CharacterOwner->GetMesh()->GetAnimInstance()->Montage_Stop(CharacterReloadMontage->BlendOut.GetBlendTime(), CharacterReloadMontage);
		}
	}

	if (EquippedRangedWeapon->GetReloadType() == EReloadType::FullClip)
	{
		GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	}

	CharacterOwner->OnActionEnded(ECharacterAction::Reload);
}

void UCharacterEquipmentComponent::CompleteReloading()
{
	CharacterOwner->OnActionEnded(ECharacterAction::Reload);
	if (!bReloading)
		return;
	
	bReloading = false;
	if (!EquippedRangedWeapon.IsValid())
		return;

	int32 ClipCapacity = EquippedRangedWeapon->GetClipCapacity();
	int32 NewAmmo = 0;
	if (EquippedRangedWeapon->IsInfiniteAmmoSupply())
	{
		NewAmmo = ClipCapacity;
	}
	else
	{
		int32 CurrentAmmo = EquippedRangedWeapon->GetAmmo();
		int32 AmmoToReload = 0;
		int32& RemainingAmmo = Pouch[(uint32)EquippedRangedWeapon->GetAmmunitionType()];
		switch (EquipmentSettings->ReloadType)
		{
			case EReloadMode::KeepUnspentAmmo:
				AmmoToReload = FMath::Min(ClipCapacity - CurrentAmmo, RemainingAmmo);
				NewAmmo = CurrentAmmo + AmmoToReload;
				break;
			case EReloadMode::DiscardUnspendAmmo:
				NewAmmo = AmmoToReload = FMath::Min(ClipCapacity, RemainingAmmo);
				break;
			default:
				break;
		}
		RemainingAmmo -= AmmoToReload;
	}
	
	EquippedRangedWeapon->SetAmmo(NewAmmo);
}

void UCharacterEquipmentComponent::ReloadInsertShells(uint8 ShellsInsertedAtOnce)
{
	if (!bReloading)
	{
		CharacterOwner->OnActionEnded(ECharacterAction::Reload);
		return;
	}

	if (!EquippedRangedWeapon.IsValid())
	{
		CharacterOwner->OnActionEnded(ECharacterAction::Reload);
		bReloading = false;
		return;
	}
	
	int32 ClipCapacity = EquippedRangedWeapon->GetClipCapacity();
	int32 CurrentAmmo = EquippedRangedWeapon->GetAmmo();
	int32& RemainingAmmo = Pouch[(uint32)EquippedRangedWeapon->GetAmmunitionType()];
	int32 AmmoToReload = FMath::Min((int32)ShellsInsertedAtOnce, RemainingAmmo);
	if (!EquippedRangedWeapon->IsInfiniteAmmoSupply())
		RemainingAmmo -= AmmoToReload;

	int32 NewAmmo = CurrentAmmo + AmmoToReload;
	EquippedRangedWeapon->SetAmmo(NewAmmo);
	if (NewAmmo == ClipCapacity)
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		UAnimInstance* WeaponAnimInstance = EquippedRangedWeapon->GetMesh()->GetAnimInstance();
		CharacterAnimInstance->Montage_JumpToSection(EquipmentSettings->ReloadMontageEndSectionName, EquippedRangedWeapon->GetCharacterReloadMontage());
		WeaponAnimInstance->Montage_JumpToSection(EquipmentSettings->ReloadMontageEndSectionName, EquippedRangedWeapon->GetWeaponReloadMontage());
		EquippedRangedWeapon->StopReloading(false);
		bReloading = false;
		CharacterOwner->OnActionEnded(ECharacterAction::Reload);
	}
}

#pragma endregion RELOAD

#pragma region FIRE MODES

void UCharacterEquipmentComponent::StartTogglingFireMode()
{
	if (!EquippedRangedWeapon.IsValid())
	{
		return;
	}

	if (!EquippedRangedWeapon->CanToggleFireMode() || bChangingEquipment || !CharacterOwner->CanStartAction(ECharacterAction::ToggleFireMode))
	{
		return;
	}

	const auto WeaponBarrelSettings = EquippedRangedWeapon->GetNextWeaponBarrelSettings();
	GetWorld()->GetTimerManager().SetTimer(ChangeFireModeTimer, this, &UCharacterEquipmentComponent::CompleteTogglingFireMode, WeaponBarrelSettings->SwitchFireModeDuration);
	if (IsValid(WeaponBarrelSettings->SwitchFireModeMontage))
	{
		CharacterOwner->PlayAnimMontageWithDuration(WeaponBarrelSettings->SwitchFireModeMontage, WeaponBarrelSettings->SwitchFireModeDuration);
	}

	if (IsValid(WeaponBarrelSettings->SwitchFireModeSFX))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), WeaponBarrelSettings->SwitchFireModeSFX, EquippedRangedWeapon->GetActorLocation());
	}
	
	EquippedRangedWeapon->StartTogglingFireMode();
	CharacterOwner->OnActionStarted(ECharacterAction::ToggleFireMode);
}

void UCharacterEquipmentComponent::InterruptTogglingFireMode()
{
	if (!bChangingEquipment || !EquippedRangedWeapon.IsValid())
	{
		bChangingEquipment = false;
		CharacterOwner->OnActionEnded(ECharacterAction::ToggleFireMode);
	}

	const auto WeaponBarrelSettings = EquippedRangedWeapon->GetNextWeaponBarrelSettings();
	GetWorld()->GetTimerManager().ClearTimer(ChangeFireModeTimer);
	if (IsValid(WeaponBarrelSettings->SwitchFireModeMontage))
	{
		CharacterOwner->StopAnimMontage(WeaponBarrelSettings->SwitchFireModeMontage);
	}
}

void UCharacterEquipmentComponent::CompleteTogglingFireMode()
{
	if (EquippedRangedWeapon.IsValid())
	{
		EquippedRangedWeapon->CompleteTogglingFireMode();
		OnActiveWeaponBarrelChanged();
		CharacterOwner->OnActionEnded(ECharacterAction::ToggleFireMode);
	}
}

#pragma endregion FIRE MODES

#pragma region THROWABLES

int UCharacterEquipmentComponent::GetActiveThrowablesCount()
{
	return Pouch[(uint32)ActiveThrowable->GetAmmoType()];
}

void UCharacterEquipmentComponent::PutActiveItemInSecondaryHand() const
{
	const auto EquippedItem = GetEquippedItem();
	if (IsValid(EquippedItem))
		EquippedItem->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquipmentSettings->SecondaryHandSocket);
}

void UCharacterEquipmentComponent::PutActiveItemInPrimaryHand() const
{
	AEquippableItem* EquippedItem = GetEquippedItem();
	if (IsValid(EquippedItem))
		EquippedItem->AttachToComponent(CharacterOwner->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, EquippedItem->GetCharacterEquippedSocketName());
}

void UCharacterEquipmentComponent::OnThrowableUsed()
{
	int32& ThrowablesCount = Pouch[(uint32)ActiveThrowable->GetAmmoType()];
	ThrowablesCountChanged.ExecuteIfBound(--ThrowablesCount);
	if (ThrowablesCount <= 0)
		ActiveThrowable->DestroyAttachedProjectile();
}

void UCharacterEquipmentComponent::DropThrowable(EThrowableSlot Slot, const FVector& WorldItemSpawnLocation)
{
	auto Item = Throwables[(uint8)Slot];
	if (!IsValid(Item))
		return;

	if (Pouch[(uint8)Item->GetAmmoType()] > 0)
	{
		auto WeaponDTR = Item->GetWeaponDTR();
		if (WeaponDTR)
		{
			FActorSpawnParameters ActorSpawnParameters;
			ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
			auto SpawnedItem = GetWorld()->SpawnActor<APickableEquipmentItem>(WeaponDTR->SpawnWorldItemClass, WorldItemSpawnLocation, FRotator::ZeroRotator,
				ActorSpawnParameters);
			SpawnedItem->SetItemDTRH(Item->GetWeaponDTRH());
			if (Item == ActiveThrowable)
			{
				ActiveThrowable->DestroyAttachedProjectile();
				ActiveThrowable.Reset();
			}
			Item->OnDropped(this, SpawnedItem);
		}
	}
	
	Item->Destroy();
}

#pragma endregion THROWABLES

EEquippableItemType UCharacterEquipmentComponent::GetEquippedItemType() const
{
	auto EquippedItem = GetEquippedItem();
	return IsValid(EquippedItem) ? EquippedItem->GetEquippableItemType() : EEquippableItemType::None;
}

void UCharacterEquipmentComponent::OnActiveWeaponBarrelChanged()
{
	if (EquippedRangedWeapon.IsValid() && WeaponAmmoInfoChangedEvent.IsBound())
	{
		WeaponAmmoInfoChangedEvent.Broadcast(EquippedRangedWeapon->IsInfiniteClip(), EquippedRangedWeapon->IsInfiniteAmmoSupply());
	}	
}

void UCharacterEquipmentComponent::OnAmmoChanged(int32 ClipAmmo) const
{
	if (EquippedRangedWeapon.IsValid())
	{
		WeaponAmmoChangedEvent.ExecuteIfBound(ClipAmmo, Pouch[(uint32)EquippedRangedWeapon->GetAmmunitionType()]);
	}
}

void UCharacterEquipmentComponent::OnOutOfAmmo()
{
	if (EquipmentSettings->bAutoReload)
		TryReload();
}

int32 UCharacterEquipmentComponent::GetAmmunationLimit(EAmmunitionType AmmoType) const
{
	int32* AmmoLimit = EquipmentSettings->AmmunitionLimits.Find(AmmoType);
	return AmmoLimit ? *AmmoLimit : -1;
}

AEquippableItem* UCharacterEquipmentComponent::GetEquippedItem() const
{
	if (EquippedRangedWeapon.IsValid())
	{
		return EquippedRangedWeapon.Get();
	}
	else if (EquippedMeleeWeapon.IsValid())
	{
		return EquippedMeleeWeapon.Get();
	}

	return nullptr;
}

void UCharacterEquipmentComponent::OnLevelDeserialized_Implementation()
{
   for(auto SavedItem:SavedLoadout)
   {
       if (IsValid(SavedItem))
       {
           TSubclassOf<AEquippableItem>PickedItem = SavedItem;
           // PickUpItem(PickedItem, GetOwner()->GetActorLocation(), true, false);
       }
   }
}

bool UCharacterEquipmentComponent::IsPreferStrafing() const
{
	auto EquippedItem = GetEquippedItem();
	return IsValid(EquippedItem) ? EquippedItem->IsPreferStrafing() : false;
}
