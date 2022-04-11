#include "ExplosiveBulletProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/Combat/ExplosionComponent.h"

AExplosiveBulletProjectile::AExplosiveBulletProjectile()
{
	bRotate = false;

	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("Explosion"));
	AddOwnedComponent(ExplosionComponent);
}

void AExplosiveBulletProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ExplosionComponent->Explode(CachedThrowerController.Get());
	Super::DestroyOnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
	// CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// StaticMeshComponent->SetVisibility(false);
	// SetLifeSpan(2.f);
}
