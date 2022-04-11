#include "BulletProjectile.h"

#include "CommonConstants.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"


ABulletProjectile::ABulletProjectile()
{
	ProjectileMovementComponent->bSimulationEnabled = true;
	ProjectileMovementComponent->bAutoActivate = false;
	bDestroyOnHit = true;
	bRotate = false;
	SetReplicateMovement(true);
}

void ABulletProjectile::LaunchProjectile(FVector Direction, float Speed, AController* Controller)
{
	StaticMeshComponent->SetVisibility(true);
	ProjectileMovementComponent->Activate();
	Super::LaunchProjectile(Direction, Speed, Controller);
}

void ABulletProjectile::BeginPlay()
{
	Super::BeginPlay();
	StaticMeshComponent->SetVisibility(false);
	// CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	CollisionComponent->SetCollisionProfileName(ProfileNoCollision);
}

void ABulletProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ProjectileMovementComponent->Deactivate();
	CollisionComponent->SetCollisionProfileName(ProfileNoCollision);
	StaticMeshComponent->SetVisibility(false);
	ProjectileHitEvent.ExecuteIfBound(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
}
