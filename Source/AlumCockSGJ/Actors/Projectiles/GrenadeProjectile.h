#pragma once

#include "CoreMinimal.h"
#include "ThrowableProjectile.h"
#include "GrenadeProjectile.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AGrenadeProjectile : public AThrowableProjectile
{
	GENERATED_BODY()

public:
	AGrenadeProjectile();

	virtual void Activate(AController* ThrowerController) override;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DetonationTime = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class USoundCue* PreExplosionSFX;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UParticleSystemComponent* ParticleSystemComponent;
	
	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
private:
	FTimerHandle DetonationTimer;
	TWeakObjectPtr<UAudioComponent> PreExplosionAudioComponent;
	void Detonate();
};
