#include "ExplosionComponent.h"

#include "CommonConstants.h"
#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "Data/DataAssets/Components/ExplosionSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"

void UExplosionComponent::BeginPlay()
{
	Super::BeginPlay();
	checkf(IsValid(ExplosionSettings), TEXT("Explosion settings DataAsset must be set"));
}

void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(GetOwner());
	// ECC_Visibility is kinda lame here
	FVector ExplosionLocation = GetOwner()->GetActorLocation();
	UGameplayStatics::ApplyRadialDamageWithFalloff(GetWorld(), ExplosionSettings->MaxDamage, ExplosionSettings->MinDamage,
		ExplosionLocation, ExplosionSettings->InnerRadius, ExplosionSettings->OuterRadius,
		ExplosionSettings->DamageFalloff, ExplosionSettings->DamageTypeClass, IgnoredActors,
		GetOwner(), Controller, ECC_Visibility);	
	
	if (ExplosionSettings->ExplosionVFX)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionSettings->ExplosionVFX, ExplosionLocation);
	
	if (ExplosionSettings->ExplosionSFX)
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ExplosionSettings->ExplosionSFX, ExplosionLocation);

	if (IsValid(ExplosionSettings->ExplosionFmodSFX))
		UFMODBlueprintStatics::PlayEventAtLocation(GetOwner(), ExplosionSettings->ExplosionFmodSFX, FTransform(ExplosionLocation), true);
	
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetOwner()->GetActorLocation(),
		ExplosionSettings->AiExplosionLoudness, GetOwner(), ExplosionSettings->AiExplosionSoundRange, SoundStimulusTag_Explosion);

	if (ExplosionEvent.IsBound())
		ExplosionEvent.Broadcast();
}