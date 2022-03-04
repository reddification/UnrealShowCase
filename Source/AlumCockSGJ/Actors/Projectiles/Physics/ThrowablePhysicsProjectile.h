#pragma once

#include "CoreMinimal.h"
#include "Actors/Projectiles/BaseThrowableProjectile.h"
#include "ThrowablePhysicsProjectile.generated.h"

UCLASS()
class ALUMCOCKSGJ_API AThrowablePhysicsProjectile : public ABaseThrowableProjectile
{
	GENERATED_BODY()

public:
	AThrowablePhysicsProjectile();
	virtual void Activate(AController* ThrowerController) override { CachedThrowerController = ThrowerController;}
	virtual void LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController) override;
	virtual void Drop(AController* ThrowerController) override;

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bDestroyOnHit = false;

	virtual void DestroyOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	TWeakObjectPtr<AController> CachedThrowerController;

	virtual void OnProjectileLaunched() { }
};
