#include "ThrowablePhysicsProjectile.h"

#include "CommonConstants.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AThrowablePhysicsProjectile::AThrowablePhysicsProjectile()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	SetRootComponent(StaticMeshComponent);
}

void AThrowablePhysicsProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (bDestroyOnHit)
	{
		StaticMeshComponent->OnComponentHit.AddDynamic(this, &AThrowablePhysicsProjectile::DestroyOnHit);
	}
}

void AThrowablePhysicsProjectile::LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController)
{
	StaticMeshComponent->SetCollisionProfileName("PhysicsActor");
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetPhysicsLinearVelocity(Direction * Speed);
	// StaticMeshComponent->AddForce(Direction * Speed);

	AActor* ProjectileOwner = GetOwner();
	while (IsValid(ProjectileOwner))
	{
		StaticMeshComponent->IgnoreActorWhenMoving(GetOwner(), true);
		ProjectileOwner = ProjectileOwner->GetOwner();
	}
	
	this->CachedThrowerController = ThrowerController; 
	OnProjectileLaunched();
}

void AThrowablePhysicsProjectile::Drop(AController* ThrowerController)
{
	CachedThrowerController = ThrowerController;
	StaticMeshComponent->SetCollisionProfileName("PhysicsActor");
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->AddForce(-FVector::UpVector * 5.f);
	AActor* ProjectileOwner = GetOwner();
	while (IsValid(ProjectileOwner))
	{
		StaticMeshComponent->IgnoreActorWhenMoving(GetOwner(), true);
		ProjectileOwner = ProjectileOwner->GetOwner();
	}
}

void AThrowablePhysicsProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}