#pragma once

#include "Data/EquipmentTypes.h"
#include "Data/UserInterfaceTypes.h"
#include "BarrelSettings.h"
#include "SaveSystem/SaveSubsystemInterface.h"
#include "WeaponBarrelSettings.generated.h"

class UAnimMontage;

UCLASS()
class ALUMCOCKSGJ_API UWeaponBarrelSettings : public UBarrelSettings, public ISaveSubsystemInterface
{
	GENERATED_BODY()
public:
	// shots per minutes
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1.f, UIMin = 1.f))
	float FireRate = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EWeaponFireMode FireMode = EWeaponFireMode::FullAuto;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations|Weapons")
	UAnimMontage* WeaponShootMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations|Weapons")
	UAnimMontage* WeaponReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations|Character")
	UAnimMontage* CharacterShootMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations|Character")
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Animations|Character")
	UAnimMontage* SwitchFireModeMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo")
	EAmmunitionType AmmunitionType = EAmmunitionType::None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1.f, UIMin = 1.f), Category="Ammo")
	int32 ClipCapacity = 30;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin=1, UIMin=1), Category="Ammo")
	int32 BulletsPerShot = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo")
	bool bInfiniteClips = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammo")
	bool bInfiniteAmmoSupply = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EReloadType ReloadType = EReloadType::FullClip;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.01f, UIMin = 0.01f), Category="Timings")
	float SwitchFireModeDuration = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 0.01f, UIMin = 0.01f), Category="Timings")
	float ReloadDuration = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* SwitchFireModeSFX;

	// bullet spread half angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin = 1.f, UIMin = 1.f, ClampMax = 10.f, UIMax = 10.f))
	float SpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim")
    bool bCanAim = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true", ClampMin = 0.f, UIMin = 0.f, ClampMax = 5.f, UIMax = 5.f))
	float AimSpreadAngle = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true", ClampMin = 1.f, UIMin = 1.f))
	float AimFOV = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true", ClampMin = 0.f, UIMin = 0.f))
	float AimMaxSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true", ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimTurnModifier = 0.5f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true", ClampMin = 0.f, UIMin = 0.f, ClampMax = 1.f, UIMax = 1.f))
	float AimLookUpModifier = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aim", meta=(EditCondition = "bCanAim == true"))
	EReticleType AimReticleType = EReticleType::Crosshair;
};
