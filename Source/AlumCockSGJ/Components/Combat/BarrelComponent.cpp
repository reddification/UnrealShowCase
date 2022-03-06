#include "BarrelComponent.h"

#include "DrawDebugHelpers.h"
#include "CommonConstants.h"
#include "DebugSubsystem.h"
#include "FMODBlueprintStatics.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Actors/Projectiles/Projectile.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/DecalComponent.h"
#include "Data/DataAssets/Items/BarrelSettings.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"

void UBarrelComponent::Shoot(const FVector& ViewLocation, const FVector& Direction, AController* ShooterController)
{
    bool bHit = false;
    switch (BarrelSettings->HitRegistrationType)
    {
    case EHitRegistrationType::HitScan:
        bHit = ShootHitScan(ViewLocation, Direction, ShooterController);
        break;
    case EHitRegistrationType::Projectile:
        ShootProjectile(ViewLocation, Direction, ShooterController);
        break;
    default:
        break;
    }
}

// TODO shoot via AnimNotify in weapon shoot montage
bool UBarrelComponent::ShootHitScan(const FVector& ViewLocation, const FVector& Direction, AController* ShooterController)
{
#if ENABLE_DRAW_DEBUG
    bool bDrawDebugEnabled = GetDebugSubsystem()->IsDebugCategoryEnabled(DebugCategoryRangeWeapons);
#else
    bool bDrawDebugEnabled = false;
#endif

    FVector ProjectileStartLocation = GetComponentLocation();
    FVector ProjectileEndLocation = ViewLocation + BarrelSettings->TraceRange * Direction;
    const UWorld* World = GetWorld();
    FHitResult ShotResult;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    CollisionQueryParams.AddIgnoredActor(GetOwner()->GetOwner());

    bool bHit = World->LineTraceSingleByChannel(ShotResult, ViewLocation, ProjectileEndLocation, ECC_Bullet, CollisionQueryParams);
    // TODO DotProduct doesnt really solve the problem of shooting behind players back. Need to fix one day
    if (bHit && FVector::DotProduct(Direction, ShotResult.ImpactPoint - ProjectileStartLocation) > 0.f)
    {
        ProjectileEndLocation = ShotResult.ImpactPoint + Direction * 5.f;
    }

    bHit = World->LineTraceSingleByChannel(ShotResult, ProjectileStartLocation, ProjectileEndLocation, ECC_Bullet, CollisionQueryParams);
    if (bHit)
    {
        ProjectileEndLocation = ShotResult.ImpactPoint;
        ApplyDamage(ShotResult, Direction, ShooterController);

        if (bDrawDebugEnabled)
        {
            DrawDebugSphere(World, ProjectileEndLocation, 10.f, 24, FColor::Red, false, 2.f);
        }

        SpawnBulletHole(ShotResult);
    }

    if (bDrawDebugEnabled)
    {
        DrawDebugLine(World, ProjectileStartLocation, ProjectileEndLocation, FColor::Red, false, 2.f);
    }

    if (IsValid(BarrelSettings->TraceFX))
    {
        UNiagaraComponent* TraceFXComponent =
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BarrelSettings->TraceFX, ProjectileStartLocation, GetComponentRotation());
        TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ProjectileEndLocation);
    }

    return bHit;
}

// TODO shoot via AnimNotify in weapon shoot montage
bool UBarrelComponent::ShootProjectile(const FVector& ViewLocation, const FVector& ViewDirection, AController* ShooterController)
{
    CachedShooterController = ShooterController;
    const float ProjectileTraceDistance = 100000.f;
    // FVector ShootDirection = (ViewLocation + ViewDirection * Range) - GetComponentLocation();
    AProjectile* CurrentProjectile = GetWorld()->SpawnActor<AProjectile>(BarrelSettings->ProjectileClass, GetComponentLocation(), FRotator::ZeroRotator);
    CurrentProjectile->SetOwner(GetOwner());
    FHitResult TraceResult;
    const FVector TraceEnd = ViewLocation + ViewDirection * ProjectileTraceDistance;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    CollisionQueryParams.AddIgnoredActor(GetDamagingActor());
    CollisionQueryParams.AddIgnoredActor(ShooterController->GetPawn());

    bool bHit = GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Bullet, CollisionQueryParams);
    // DrawDebugLine(GetWorld(), ViewLocation, TraceEnd, FColor::Purple, false, 5, 0, 2);
    FVector ShootDirection = bHit || TraceResult.bBlockingHit
                                 ? (TraceResult.ImpactPoint - CurrentProjectile->GetActorLocation()).GetSafeNormal()
                                 : (TraceEnd - CurrentProjectile->GetActorLocation()).GetSafeNormal();
    // : (TraceEnd - ViewLocation).GetSafeNormal();
    CurrentProjectile->SetActorRotation(ShootDirection.ToOrientationRotator());
    CurrentProjectile->ProjectileHitEvent.BindUObject(this, &UBarrelComponent::OnProjectileHit);

    CurrentProjectile->LaunchProjectile(ShootDirection, GetOwner()->GetVelocity().Size() + BarrelSettings->ProjectileSpeed, ShooterController);
    return true;
}

void UBarrelComponent::OnProjectileHit(const FHitResult& HitResult, const FVector& Direction)
{
    if (CachedShooterController.IsValid())
    {
        ApplyDamage(HitResult, Direction, CachedShooterController.Get());
    }

    SpawnBulletHole(HitResult);
}

void UBarrelComponent::ApplyDamage(const FHitResult& ShotResult, const FVector& Direction, AController* ShooterController) const
{
    AActor* HitActor = ShotResult.GetActor();
    // Perhaps its better to use squared distance
    // TODO do something for projectiles
    float Damage = IsValid(BarrelSettings->DamageFalloffDiagram)
        ? BarrelSettings->DamageFalloffDiagram->GetFloatValue(ShotResult.Distance / BarrelSettings->TraceRange) * BarrelSettings->InitialDamage
        : BarrelSettings->InitialDamage;

    if (IsValid(HitActor))
    {
        FPointDamageEvent DamageEvent;
        DamageEvent.HitInfo = ShotResult;
        DamageEvent.ShotDirection = Direction;
        DamageEvent.DamageTypeClass = BarrelSettings->DamageTypeClass;
        HitActor->TakeDamage(Damage, DamageEvent, ShooterController, GetDamagingActor());
    }
}

void UBarrelComponent::SpawnBulletHole(const FHitResult& HitResult)
{
    UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
        GetWorld(), BarrelSettings->DecalSettings.Material, BarrelSettings->DecalSettings.Size, HitResult.ImpactPoint, HitResult.ImpactNormal.ToOrientationRotator());
    if (IsValid(DecalComponent))
    {
        DecalComponent->SetFadeOut(BarrelSettings->DecalSettings.LifeTime, BarrelSettings->DecalSettings.FadeOutTime);
        DecalComponent->SetFadeScreenSize(0.0001f);
    }
}

void UBarrelComponent::FinalizeShot()
{
    if (IsValid(BarrelSettings->MuzzleFlashFX))
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BarrelSettings->MuzzleFlashFX, GetComponentLocation(), GetComponentRotation());

    if (IsValid(BarrelSettings->ShotSound))
        UGameplayStatics::SpawnSoundAttached(BarrelSettings->ShotSound, GetAttachmentRoot());

    if (IsValid(BarrelSettings->FmodSoundEvent))
    {
        if (FmodPlayingOwner != nullptr)
        {
            FmodPlayingOwner->PlayFmodEvent(BarrelSettings->FmodSoundEvent);
        }
        else
        {
            UFMODBlueprintStatics::PlayEventAttached(BarrelSettings->FmodSoundEvent, this, NAME_None, FVector::ZeroVector,
            EAttachLocation::KeepRelativeOffset, true, true, true);
        }
    }
    
    // TODO tweak params
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetComponentLocation(), BarrelSettings->AiShotLoudness,
        GetOwner(), BarrelSettings->AiShotSoundRange, SoundStimulusTag_Shot);
}

void UBarrelComponent::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(BarrelSettings), TEXT("BarrelSettings must be set"));
    FmodPlayingOwner = GetOwner();
}

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG

const UDebugSubsystem* UBarrelComponent::GetDebugSubsystem() const
{
    if (!IsValid(DebugSubsystem))
    {
        DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
    }

    return DebugSubsystem;
}

#endif
