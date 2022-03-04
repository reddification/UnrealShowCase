// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ThrowablePhysicsProjectile.h"
#include "ExplosivePhysicsProjectile.generated.h"

/**
 * 
 */
UCLASS()
class ALUMCOCKSGJ_API AExplosivePhysicsProjectile : public AThrowablePhysicsProjectile
{
	GENERATED_BODY()

public:
	AExplosivePhysicsProjectile();

	virtual void Activate(AController* ThrowerController) override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UExplosionComponent* ExplosionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class URadialForceComponent* RadialForceComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UParticleSystemComponent* ParticleSystemComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DetonationTime = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* PreExplosionSFX;

	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:
	FTimerHandle DetonationTimer;
	TWeakObjectPtr<UAudioComponent> PreExplosionAudioComponent;
	void Detonate();
};
