#include "ThrowableProjectile.h"

#include "CommonConstants.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/RotatingMovementComponent.h"

AThrowableProjectile::AThrowableProjectile()
{
	CollisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("ProjectileCollision"));
	SetRootComponent(CollisionComponent);

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->bSimulationEnabled = false;
	AddOwnedComponent(ProjectileMovementComponent);

	RotatingMovementComponent = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovement"));
	AddOwnedComponent(RotatingMovementComponent);
	
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(CollisionComponent);
}

void AThrowableProjectile::BeginPlay()
{
	Super::BeginPlay();
	if (bDestroyOnHit)
	{
		CollisionComponent->OnComponentHit.AddDynamic(this, &AThrowableProjectile::DestroyOnHit);
		// StaticMeshComponent->OnComponentHit.AddDynamic(this, &AThrowableProjectile::DestroyOnHit);
	}
}

void AThrowableProjectile::LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController)
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
		ProjectileMovementComponent->bRotationFollowsVelocity = 0;
		ProjectileMovementComponent->bInterpRotation = 1;
		RotatingMovementComponent->RotationRate = RotationRate;
	}
	
	ProjectileMovementComponent->OnProjectileBounce.AddDynamic(this, &AThrowableProjectile::OnProjectileBounced);
}

void AThrowableProjectile::Drop(AController* ThrowerController)
{
	CachedThrowerController = ThrowerController;
	ProjectileMovementComponent->Velocity = -FVector::UpVector * 5.f;
	ProjectileMovementComponent->bSimulationEnabled = true;
	CollisionComponent->SetCollisionProfileName(ProfileProjectile);
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
}

void AThrowableProjectile::DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
}

void AThrowableProjectile::OnProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
	if (bRotate && !ProjectileMovementComponent->bRotationFollowsVelocity)
	{
		RotatingMovementComponent->RotationRate = FRotator::ZeroRotator;
		ProjectileMovementComponent->bRotationFollowsVelocity = 1;
		ProjectileMovementComponent->bInterpRotation = 1;
	}
}