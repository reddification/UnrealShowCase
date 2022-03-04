#include "EnvironmentDamage.h"

UEnvironmentDamage::UEnvironmentDamage(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DamageImpulse = 0.0f;
	DestructibleImpulse = 0.0f;
	DestructibleDamageSpreadScale = 0.0f;
	bScaleMomentumByMass = false;
	DamageFalloff = 1.0f;
	bCausedByWorld = 1;
}

FDamageEvent GetEnvironmentDamageEvent()
{
	FDamageEvent DamageEvent;
	DamageEvent.DamageTypeClass = TSubclassOf<UEnvironmentDamage>();
	return DamageEvent;
}
