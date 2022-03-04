#include "ExplosivePhysicsProjectile.h"

#include "Components/AudioComponent.h"
#include "Components/Combat/ExplosionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Sound/SoundCue.h"

AExplosivePhysicsProjectile::AExplosivePhysicsProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("Explosion"));
	ExplosionComponent->SetupAttachment(RootComponent);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystemComponent->SetupAttachment(RootComponent);

	RadialForceComponent = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForce"));
	RadialForceComponent->SetupAttachment(RootComponent);
}

void AExplosivePhysicsProjectile::Activate(AController* ThrowerController)
{
	Super::Activate(ThrowerController);
	GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosivePhysicsProjectile::Detonate, DetonationTime);
	PreExplosionAudioComponent = UGameplayStatics::SpawnSoundAttached(PreExplosionSFX, StaticMeshComponent);
	ParticleSystemComponent->Activate();
}

void AExplosivePhysicsProjectile::BeginPlay()
{
	Super::BeginPlay();
	ParticleSystemComponent->Deactivate();
}

void AExplosivePhysicsProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                               UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Detonate();
}

void AExplosivePhysicsProjectile::Detonate()
{
	if (CachedThrowerController.IsValid())
	{
		ExplosionComponent->Explode(CachedThrowerController.Get());
	}

	RadialForceComponent->FireImpulse();
	ParticleSystemComponent->Deactivate();
	StaticMeshComponent->SetVisibility(false);
	if (PreExplosionAudioComponent.IsValid())
	{
		PreExplosionAudioComponent->Stop();
	}

	SetLifeSpan(2.);
}
