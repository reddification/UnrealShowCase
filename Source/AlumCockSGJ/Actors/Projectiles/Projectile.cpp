#include "Actors/Projectiles/Projectile.h"

#include "CommonConstants.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AProjectile::AProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	
	SetRootComponent(CollisionComponent);
	// CollisionComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	CollisionComponent->SetCollisionProfileName(ProfileNoCollision);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->bSimulationEnabled = false;
	AddOwnedComponent(ProjectileMovementComponent);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionProfileName(ProfileNoCollision);
	
	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	AddOwnedComponent(RotatingMovementComponent);
	SetReplicates(true);
	// bReplicates = 1;
	// bAlwaysRelevant = 1;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (bDestroyOnHit)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectile::DestroyOnHit);
	}
	
	if (bRotate)
	{
		ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AProjectile::OnProjectileStopped);
		ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AProjectile::OnProjectileBounced);
	}
}

void AProjectile::Activate(AController* ThrowerController)
{
	CachedThrowerController = ThrowerController;
}

void AProjectile::LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController)
{
	ProjectileMovementComponent->Velocity = Direction * Speed;
	ProjectileMovementComponent->bSimulationEnabled = true;
	CollisionComponent->SetCollisionProfileName(ProfileProjectile);
	AActor* ProjectileOwner = GetOwner();
	while (IsValid(ProjectileOwner))
	{
		CollisionComponent->IgnoreActorWhenMoving(ProjectileOwner, true);
		ProjectileOwner = ProjectileOwner->GetOwner();
	}
	
	this->CachedThrowerController = ThrowerController; 
	OnProjectileLaunched();

	if (bRotate)
	{
		RotatingMovementComponent->RotationRate = FRotator(BaseRotationRate.Pitch * Direction.Y,
			BaseRotationRate.Yaw * Direction.Z, BaseRotationRate.Roll * Direction.X) * ProjectileMovementComponent->Velocity.Size();
	}
	else
	{
		ProjectileMovementComponent->bRotationFollowsVelocity = 1;
	}
}

void AProjectile::Drop(AController* ThrowerController)
{
	CachedThrowerController = ThrowerController;
	ProjectileMovementComponent->Velocity = -FVector::UpVector * 5.f;
	ProjectileMovementComponent->bSimulationEnabled = true;
	CollisionComponent->SetCollisionProfileName(ProfileProjectile);
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
}

void AProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ProjectileHitEvent.ExecuteIfBound(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComponent->SetVisibility(false);
	SetLifeSpan(2.f); 	
}

void AProjectile::OnProjectileStopped(const FHitResult& ImpactResult)
{
}

void AProjectile::OnProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (bRotate)
	{
		RotatingMovementComponent->RotationRate = FRotator(BaseRotationRate.Pitch * ImpactResult.ImpactNormal.Y,
		BaseRotationRate.Yaw * ImpactResult.ImpactNormal.Z, BaseRotationRate.Roll * ImpactResult.ImpactNormal.X)
			* ImpactVelocity.Size();
	}
}