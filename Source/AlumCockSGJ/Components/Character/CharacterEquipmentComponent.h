#pragma once

#include "CoreMinimal.h"
#include "CharacterAttributesComponent.h"
#include "Actors/CommonDelegates.h"
#include "Components/ActorComponent.h"
#include "Data/EquipmentData.h"
#include "Data/EquipmentTypes.h"
#include "Data/UserInterfaceTypes.h"
#include "Data/DataAssets/Components/EquipmentSettings.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "CharacterEquipmentComponent.generated.h"

class AEquippableItem;
class ARangeWeaponItem;
class AMeleeWeaponItem;
class AThrowableItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FWeaponEquippedEvent, const FText& Name, EReticleType Reticle)
DECLARE_MULTICAST_DELEGATE(FWeaponUnequippedEvent);
DECLARE_DELEGATE_TwoParams(FChangingEquippedItemStarted, UAnimMontage* Montage, float Duration)

DECLARE_DELEGATE_OneParam(FAimingSpeedChangedEvent, float NewAimSpeed)
DECLARE_DELEGATE(FAimingSpeedResetEvent)
DECLARE_DELEGATE(FMeleeWeaponEquippedEvent)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUMCOCKSGJ_API UCharacterEquipmentComponent : public UActorComponent, public ISaveSubsystemInterface
{
	GENERATED_BODY()

friend AEquippableItem;
friend ARangeWeaponItem;
friend AMeleeWeaponItem;
friend AThrowableItem;
	
public:
	UCharacterEquipmentComponent();
	EEquippableItemType GetEquippedItemType() const;
	
	bool IsPreferStrafing() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& ReplicatedProps) const override;
	
	ARangeWeaponItem* GetCurrentRangeWeapon() const { return EquippedRangedWeapon.Get(); }
	AMeleeWeaponItem* GetCurrentMeleeWeapon() const { return EquippedMeleeWeapon.Get(); }
	AThrowableItem* GetCurrentThrowable() const { return ActiveThrowable.Get(); }

	void TryReload();
	void InterruptReloading();
	bool IsAutoReload() const { return EquipmentSettings ? EquipmentSettings->bAutoReload : false; }
	
	bool IsReloading() const { return bReloading; }
	int GetActiveThrowablesCount();
	void DropWeapon(EEquipmentSlot Slot, const FVector& WorldItemSpawnLocation);
	void DropThrowable(EThrowableSlot Slot, const FVector& WorldItemSpawnLocation);
	void DestroyEquipment();
	void PickNextWeapon() { EquipItem(1); }
	void PickPreviousWeapon() { EquipItem(-1); }

	void EquipPrimaryThrowable();
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	void EquipMeleeWeapon();
	void EquipSecondaryThrowable();
	bool IsWeaponEquipped() const;
	bool IsThrowableEquipped() const;

	mutable FWeaponEquippedEvent WeaponEquippedEvent;
	mutable FWeaponUnequippedEvent WeaponUnequippedEvent;
	mutable FWeaponAmmoChangedEvent WeaponAmmoChangedEvent;
	mutable FWeaponAmmoInfoChangedEvent WeaponAmmoInfoChangedEvent;
	mutable FThrowablesCountChanged ThrowablesCountChanged;
	mutable FThrowableEquippedEvent ThrowableEquippedEvent;
	mutable FAimingSpeedChangedEvent AimingSpeedChangedEvent;
	mutable FAimingSpeedResetEvent AimingSpeedResetEvent;
	mutable FMeleeWeaponEquippedEvent MeleeWeaponEquippedEvent;

	void CreateLoadout();

	bool PickUpItem(const TSubclassOf<AEquippableItem>& ItemType, const FPickUpItemData& PickUpItemData);
	// bool PickUpThrowable(const TSubclassOf<AEquippableItem>& ThrowableItemClass, const FPickUpItemData& PickUpData);
	bool PickUpAmmo(EAmmunitionType AmmoType, int Ammo, bool bThrowable);
	
	void EquipItem(EEquipmentSlot NewSlot);
	void EquipThrowable(EThrowableSlot ThrowableType);

	void OnWeaponsChanged();
	void EquipItem(int delta);
	void InterruptChangingEquipment();
	bool CanReload();
	void UnequipItem(bool bForce);

	bool IsChangingEquipment() const { return bChangingEquipment; }

	void ReloadInsertShells(uint8 ShellsInsertedAtOnce);
	void StartTogglingFireMode();
	void InterruptTogglingFireMode();

	void PutActiveItemInSecondaryHand() const;
	void PutActiveItemInPrimaryHand() const;
	void OnThrowableUsed();

	AEquippableItem* GetEquippedItem() const;

    virtual void OnLevelDeserialized_Implementation() override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,SaveGame)
	class UEquipmentSettings* EquipmentSettings;

private:
	TObjectPtr<ARangeWeaponItem> EquippedRangedWeapon = nullptr;
	TObjectPtr<AThrowableItem> ActiveThrowable = nullptr;
	TObjectPtr<AMeleeWeaponItem> EquippedMeleeWeapon = nullptr;

    UPROPERTY(SaveGame, ReplicatedUsing=OnRep_OnEquippedSlot)
	EEquipmentSlot EquippedSlot;
	
	EEquipmentSlot PreviousEquippedSlot = EEquipmentSlot::None;
	EThrowableSlot EquippedThrowableSlot = EThrowableSlot::None;

	UPROPERTY()
	class ABaseCharacter* CharacterOwner;

	UFUNCTION(Server, Reliable)
	void Server_EquipItemInSlot(EEquipmentSlot Slot);

	UFUNCTION()
	void OnRep_OnEquippedSlot(EEquipmentSlot PreviousSlot);
	
	void CompleteReloading();
	void CompleteTogglingFireMode();
	void OnActiveWeaponBarrelChanged();
	void OnAmmoChanged(int32 ClipAmmo) const;
	void OnOutOfAmmo();
	int32 GetAmmunationLimit(EAmmunitionType AmmoType) const;
	
    UPROPERTY(SaveGame)
    TArray<UClass*> SavedLoadout;
    
    UPROPERTY(Replicated)
	TArray<int32> Pouch;
	
    UPROPERTY(ReplicatedUsing=OnRep_Loadout)
	TArray<AEquippableItem*> Loadout;

	UPROPERTY(ReplicatedUsing=OnRep_Throwables)
	TArray<AThrowableItem*> Throwables;

	UFUNCTION()
	void OnRep_Loadout();

	UFUNCTION()
	void OnRep_Throwables();

	bool bReloading = false;
	bool bChangingEquipment = false;

	UPROPERTY(SaveGame)
	FEquipmentData ActiveEquippingData;
		
	FTimerHandle ReloadTimer;
	FTimerHandle ChangingEquipmentTimer;
	FTimerHandle ChangeFireModeTimer;
};
