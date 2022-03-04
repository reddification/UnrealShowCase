#pragma once

#include "CoreMinimal.h"
#include "DebugSubsystem.h"
#include "Components/SceneComponent.h"
#include "Data/DecalSettings.h"
#include "FMODEvent.h"
#include "Interfaces/FmodPlayingActor.h"
#include "BarrelComponent.generated.h"

class UNiagaraSystem;

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
    HitScan,
    Projectile
};

UCLASS(ClassGroup = (Custom), Abstract)
class ALUMCOCKSGJ_API UBarrelComponent : public USceneComponent
{
    GENERATED_BODY()

public:
    virtual void Shoot(const FVector& ViewLocation, const FVector& Direction, AController* ShooterController);
    virtual void ApplyDamage(const FHitResult& ShotResult, const FVector& Direction, AController* ShooterController) const;
    virtual void FinalizeShot();

    int32 GetAmmo() const { return Ammo; }
    void SetAmmo(int32 NewValue) { Ammo = NewValue; }

protected:
    virtual void BeginPlay() override;

    virtual AActor* GetDamagingActor() const { return GetOwner(); }

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    class UBarrelSettings* BarrelSettings;

private:
    bool ShootHitScan(const FVector& ViewLocation, const FVector& Direction, AController* ShooterController);
    bool ShootProjectile(const FVector& ViewLocation, const FVector& ViewDirection, AController* ShooterController);
    void SpawnBulletHole(const FHitResult& HitResult);
    void OnProjectileHit(const FHitResult& HitResult, const FVector& Direction);
    
    TWeakObjectPtr<AController> CachedShooterController = nullptr;

    TScriptInterface<IFmodPlayingActor> FmodPlayingOwner;
    
    int32 Ammo = 0;

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
    const UDebugSubsystem* GetDebugSubsystem() const;
    mutable UDebugSubsystem* DebugSubsystem;
#endif
};
