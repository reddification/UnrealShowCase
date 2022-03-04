#pragma once

#include "ExplosionSettings.generated.h"

UCLASS()
class ALUMCOCKSGJ_API UExplosionSettings : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 1, ClampMin = 1))
	float MaxDamage = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 1, ClampMin = 1))
	float MinDamage = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0))
	float DamageFalloff = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0))
	float InnerRadius = 1000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0))
	float OuterRadius = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0), Category="SFX|AI")
	float AiExplosionLoudness = 2000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(UIMin = 0, ClampMin = 0), Category="SFX|AI")
	float AiExplosionSoundRange = 2000.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UParticleSystem* ExplosionVFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* ExplosionSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UFMODEvent* ExplosionFmodSFX;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UDamageType> DamageTypeClass;
};
