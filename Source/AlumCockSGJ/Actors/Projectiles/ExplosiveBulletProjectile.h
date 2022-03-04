#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ExplosiveBulletProjectile.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AExplosiveBulletProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	AExplosiveBulletProjectile();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UExplosionComponent* ExplosionComponent;

	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		FVector NormalImpulse, const FHitResult& Hit) override;
};
