#pragma once

#include "CoreMinimal.h"
#include "NiagaraComponent.h"
#include "Actors/CommonDelegates.h"
#include "Actors/Equipment/EquippableItem.h"
#include "Actors/Interactive/Pickables/PickableEquipmentItem.h"
#include "Components/Character/CharacterEquipmentComponent.h"
#include "Components/Combat/WeaponBarrelComponent.h"
#include "Data/EquipmentTypes.h"
#include "Data/DataAssets/Items/WeaponBarrelSettings.h"
#include "Interfaces/AudioActor.h"
#include "RangeWeaponItem.generated.h"

class UAnimMontage;

DECLARE_MULTICAST_DELEGATE_OneParam(FShootEvent, UAnimMontage* CharacterShootMontage)
DECLARE_DELEGATE(FOutOfAmmoEvent)

class UAnimMontage;

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API ARangeWeaponItem : public AEquippableItem, public IAudioActor
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	bool TryStartFiring(AController* ShooterController);
	void StopFiring();

	bool StartAiming();
	void StopAiming();
	void StartReloading(const float DesiredReloadDuration);
	void StopReloading(bool bInterrupted);

	UAnimMontage* GetCharacterShootMontage() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->CharacterShootMontage; }
	UAnimMontage* GetCharacterReloadMontage() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->CharacterReloadMontage; }
	const UAnimMontage* GetWeaponReloadMontage() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->WeaponReloadMontage; }

	FTransform GetForegripTransform() const;
	EReloadType GetReloadType() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->ReloadType; }
	USkeletalMeshComponent* GetMesh() { return WeaponMeshComponent; }
	EReticleType GetAimReticleType() const {return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimReticleType; }

	void StartTogglingFireMode();
	bool CanToggleFireMode() const { return Barrels.Num() > 1 && !(bFiring || bChangingFireMode); }
	void CompleteTogglingFireMode();
	const UWeaponBarrelSettings* GetNextWeaponBarrelSettings () const;
	bool CanReload() const { return !bChangingFireMode; }
	bool IsFiring() const { return bFiring; }
	bool IsInfiniteAmmoSupply() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->bInfiniteAmmoSupply; }
	bool IsInfiniteClip() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->bInfiniteClips; }
	virtual EAmmunitionType GetAmmoType() const override { return PrimaryWeaponBarrelComponent->GetWeaponBarrelSettings()->AmmunitionType; }

	mutable FShootEvent ShootEvent;
	mutable FAmmoChangedEvent AmmoChangedEvent;
	mutable FOutOfAmmoEvent OutOfAmmoEvent;
	
	float GetAimFOV() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimFOV; }
	float GetAimMaxSpeed() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimMaxSpeed; }
	float GetAimTurnModifier() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimTurnModifier; }
	float GetAimLookUpModifier() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimLookUpModifier; }

	int32 GetAmmo() const { return ActiveWeaponBarrel->GetAmmo(); }
	void SetAmmo(int32 NewAmmo);
	int32 GetClipCapacity() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->ClipCapacity; }
	EAmmunitionType GetAmmunitionType() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->AmmunitionType; }

	float GetReloadDuration() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->ReloadDuration; }

	virtual EReticleType GetReticleType() const override;

	const class UCameraComponent* GetScopeCameraComponent() const { return ScopeCameraComponent; }

	bool CanAim() const { return ActiveWeaponBarrel->GetWeaponBarrelSettings()->bCanAim; }

	virtual bool TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData&) override;
	virtual void OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent) override;
	virtual void OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent) override;

	virtual void OnDropped(UCharacterEquipmentComponent* EquipmentComponent, APickableEquipmentItem* PickableEquipmentItem) override;

	virtual float PlaySound(USoundCue* Sound) override;
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class USkeletalMeshComponent* WeaponMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UWeaponBarrelComponent* PrimaryWeaponBarrelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UCameraComponent* ScopeCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UAudioComponent* AudioComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName MuzzleSocketName = "muzzle_socket";
	
private:
	FTimerHandle ShootTimer;
	FTimerHandle ChangeFireModeTimer;
	
	bool bAiming = false;
	bool bReloading = false;
	bool bFiring = false;
	bool bChangingFireMode = false;

	UPROPERTY()
	class AController* CachedShooterController = nullptr;

	float PlayAnimMontage(UAnimMontage* AnimMontage, float DesiredDuration = -1);
	float GetShootTimerInterval() const { return 60.f / ActiveWeaponBarrel->GetWeaponBarrelSettings()->FireRate; };
	bool Shoot();
	void ResetShot();
	FVector GetBulletSpreadOffset(const FRotator& ShotOrientation) const;
	float GetBulletSpreadAngleRad () const;

	// Fire modes to cycle through
	UPROPERTY()
	TArray<UWeaponBarrelComponent*> Barrels;
	
	TWeakObjectPtr<UWeaponBarrelComponent> ActiveWeaponBarrel;
	int ActiveBarrelIndex = 0;

	TWeakObjectPtr<const class URangeWeaponSettings> RangeWeaponSettings;
};
