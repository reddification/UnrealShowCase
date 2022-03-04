#pragma once

#include "CoreMinimal.h"
#include "BaseThrowableProjectile.h"
#include "ThrowableProjectile.generated.h"

UCLASS(Blueprintable)
class ALUMCOCKSGJ_API AThrowableProjectile : public ABaseThrowableProjectile
{
	GENERATED_BODY()

public:
	AThrowableProjectile();

	virtual void Activate(AController* ThrowerController) override { CachedThrowerController = ThrowerController;}

	virtual void LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController) override;
	virtual void Drop(AController* ThrowerController) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCapsuleComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URotatingMovementComponent* RotatingMovementComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator RotationRate = FRotator(15.f, 30.f, 45.f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDestroyOnHit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRotate = false;
	
	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<AController> CachedThrowerController;

	UFUNCTION()
	virtual void OnProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
	
	virtual void OnProjectileLaunched() { };
};
