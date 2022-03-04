#include "Actors/Projectiles/GrenadeProjectile.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/Combat/ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

AGrenadeProjectile::AGrenadeProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("Explosion"));
	ExplosionComponent->SetupAttachment(RootComponent);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystemComponent->SetupAttachment(CollisionComponent);
}

void AGrenadeProjectile::Activate(AController* ThrowerController)
{
	Super::Activate(ThrowerController);
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AGrenadeProjectile::Detonate, DetonationTime);
	PreExplosionAudioComponent = UGameplayStatics::SpawnSoundAttached(PreExplosionSFX, StaticMeshComponent);
	ParticleSystemComponent->Activate();
}

void AGrenadeProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Detonate();
}

void AGrenadeProjectile::Detonate()
{
	if (CachedThrowerController.IsValid())
	{
		ExplosionComponent->Explode(CachedThrowerController.Get());
	}

	ParticleSystemComponent->Deactivate();
	StaticMeshComponent->SetVisibility(false);
	if (PreExplosionAudioComponent.IsValid())
	{
		PreExplosionAudioComponent->Stop();
	}
	
	SetLifeSpan(2.);
}
