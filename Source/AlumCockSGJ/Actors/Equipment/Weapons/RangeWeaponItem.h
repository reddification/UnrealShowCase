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

DECLARE_DELEGATE(FOutOfAmmoEvent)

USTRUCT(BlueprintType)
struct FShotInfo
{
	GENERATED_BODY()

public:
	FShotInfo(const FVector& Location, const FVector& Direction) : Location_x10(Location * 10.f), Direction(Direction) { }
	FShotInfo() :Location_x10(FVector_NetQuantize100::ZeroVector), Direction(FVector_NetQuantizeNormal::ZeroVector) { }

	FVector GetLocation() const { return Location_x10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
    
private:
	UPROPERTY()
	FVector_NetQuantize100 Location_x10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;
};


class UAnimMontage;

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API ARangeWeaponItem : public AEquippableItem, public IAudioActor
{
	GENERATED_BODY()
	
public:
	ARangeWeaponItem();

	bool TryStartFiring();
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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void RegisterOnClient(UCharacterEquipmentComponent* EquipmentComponent) override;

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
	bool bCanFire = true;
	bool bChangingFireMode = false;

	float PlayAnimMontage(UAnimMontage* AnimMontage, float DesiredDuration = -1);
	float GetShootTimerInterval() const { return 60.f / ActiveWeaponBarrel->GetWeaponBarrelSettings()->FireRate; };
	bool Shoot();
	void ResetShot();
	FVector GetBulletSpreadOffset(const FRotator& ShotOrientation) const;
	float GetBulletSpreadAngleRad () const;

	// Fire modes to cycle through
	UPROPERTY()
	TArray<UWeaponBarrelComponent*> Barrels;
	
	TObjectPtr<UWeaponBarrelComponent> ActiveWeaponBarrel;
	int ActiveBarrelIndex = 0;

	TObjectPtr<const class URangeWeaponSettings> RangeWeaponSettings;

	UPROPERTY(ReplicatedUsing=OnRep_Shots)
	TArray<FShotInfo> ReplicatedShots;

	UFUNCTION()
	void OnRep_Shots();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_Shoot(const TArray<FShotInfo>& Shots);

	bool Server_Shoot_Validate(const TArray<FShotInfo>& Shots);
	
	void ShootInternal(const TArray<FShotInfo>& Shots);
};
