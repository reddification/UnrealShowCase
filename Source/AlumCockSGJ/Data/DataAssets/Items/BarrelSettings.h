#pragma once
#include "Components/Combat/BarrelComponent.h"
#include "Data/DecalSettings.h"

#include "BarrelSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UBarrelSettings : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "HitRegistrationType == EHitRegistrationType::HitScan"))
	float TraceRange = 5000.f;

	// per bullet or projectile. If Amount of bullets per shot > 1 (shotgun for example) then this damage is applied for each
	// hitscan/projectile
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float InitialDamage = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* TraceFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FDecalSettings DecalSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EHitRegistrationType HitRegistrationType = EHitRegistrationType::HitScan;

	// MUst be normalized 0..1 on both axis. Curve value will be multiplied by InitialDamage and applied to actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveFloat* DamageFalloffDiagram;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (EditCondition = "HitRegistrationType == EHitRegistrationType::Projectile"))
	TSubclassOf<class AProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
		meta = (ClampMin = 200.f, UIMin = 200.f, EditCondition = "HitRegistrationType == EHitRegistrationType::Projectile"))
	float ProjectileSpeed = 2000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SFX")
	class USoundCue* ShotSound;

	UPROPERTY(EditAnywhere, Category = "SFX")
	class UFMODEvent* FmodSoundEvent;

	UPROPERTY(EditAnywhere, Category = "SFX")
	class UFMODEvent* ShootWithNoAmmoSFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0), Category="SFX|AI")
	float AiShotLoudness = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0), Category="SFX|AI")
	float AiShotSoundRange = 3000.f;
};