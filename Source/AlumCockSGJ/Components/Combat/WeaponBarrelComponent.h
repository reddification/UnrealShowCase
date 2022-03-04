#pragma once

#include "CoreMinimal.h"
#include "BarrelComponent.h"
#include "WeaponBarrelComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ALUMCOCKSGJ_API UWeaponBarrelComponent : public UBarrelComponent
{
	GENERATED_BODY()

public:
	const class UWeaponBarrelSettings* GetWeaponBarrelSettings() const;
	
protected:
	virtual void BeginPlay() override;
	virtual AActor* GetDamagingActor() const override;

private:
	TWeakObjectPtr<const class UWeaponBarrelSettings> WeaponBarrelSettings;
};
