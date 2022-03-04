// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

DECLARE_DELEGATE_TwoParams(FProjectileHitEvent, const FHitResult& HitResult, const FVector& ShotDirection);

UCLASS()
class ALUMCOCKSGJ_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	mutable FProjectileHitEvent ProjectileHitEvent;

	void LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController);
	
	virtual void Activate(AController* ThrowerController) { CachedThrowerController = ThrowerController; }
	void Drop(AController* ThrowerController);

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	    FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URotatingMovementComponent* RotatingMovementComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDestroyOnHit = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bRotate = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FRotator BaseRotationRate;
	
	virtual void OnProjectileLaunched() {}

	TWeakObjectPtr<AController> CachedThrowerController;

private:
	UFUNCTION()
	void OnProjectileStopped(const FHitResult& ImpactResult);

	UFUNCTION()
	void OnProjectileBounced(const FHitResult& ImpactResult, const FVector& ImpactVelocity);
};
