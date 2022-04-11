#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GameFramework/Actor.h"
#include "BulletProjectile.generated.h"

UCLASS()
class ALUMCOCKSGJ_API ABulletProjectile : public AProjectile
{
	GENERATED_BODY()

public:
	ABulletProjectile();

	virtual void LaunchProjectile(FVector Direction, float Speed, AController* Controller) override;
	
protected:
	virtual void BeginPlay() override;

	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
};
