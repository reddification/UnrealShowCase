#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseThrowableProjectile.generated.h"

UCLASS(Abstract, NotBlueprintable)
class ALUMCOCKSGJ_API ABaseThrowableProjectile : public AActor
{
	GENERATED_BODY()

public:
	virtual void Activate(AController* ThrowerController) { bIsActive = true; }
	virtual void LaunchProjectile(FVector Direction, float Speed, AController* ThrowerController) { }
	virtual void Drop(AController* ThrowerController) { }
	bool IsActive() const { return bIsActive; }

protected:
	bool bIsActive = false;
};
