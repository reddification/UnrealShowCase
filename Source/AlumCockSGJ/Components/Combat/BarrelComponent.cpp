#include "BarrelComponent.h"

#include "DrawDebugHelpers.h"
#include "CommonConstants.h"
#include "DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Actors/Projectiles/Projectile.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/DecalComponent.h"
#include "Data/DataAssets/Items/BarrelSettings.h"
#include "Net/UnrealNetwork.h"
#include "Perception/AISense_Hearing.h"
#include "Sound/SoundCue.h"

UBarrelComponent::UBarrelComponent()
{
    SetIsReplicatedByDefault(true);
}

void UBarrelComponent::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(BarrelSettings), TEXT("BarrelSettings must be set"));
    AudioActorOwner = GetOwner();
    if (BarrelSettings->HitRegistrationType == EHitRegistrationType::Projectile && IsValid(BarrelSettings->ProjectileClass) && GetOwnerRole() == ROLE_Authority)
    {
        ProjectilesPool.Reserve(BarrelSettings->ProjectilePoolSize);
        for (int i = 0; i < BarrelSettings->ProjectilePoolSize; i++)
        {
            FActorSpawnParameters ActorSpawnParameters;
            ActorSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(BarrelSettings->ProjectileClass, ProjectilesPoolLocation,
                FRotator::ZeroRotator, ActorSpawnParameters);
            Projectile->SetOwner(GetDamagingActor());
            // Projectile->SetActorEnableCollision(false);
            Projectile->ProjectileHitEvent.BindUObject(this, &UBarrelComponent::OnProjectileHit);
            ProjectilesPool.Emplace(Projectile);
        }
    }
}

void UBarrelComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(UBarrelComponent, ProjectilesPool)
    DOREPLIFETIME(UBarrelComponent, CurrentProjectileIndex)
}

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
            DrawDebugSphere(World, ProjectileEndLocation, 10.f, 24, FColor::Red, false, 2.f);

        SpawnBulletHole(ShotResult);
    }

    if (bDrawDebugEnabled)
        DrawDebugLine(World, ProjectileStartLocation, ProjectileEndLocation, FColor::Red, false, 2.f);

    if (IsValid(BarrelSettings->TraceFX))
    {
        UNiagaraComponent* TraceFXComponent =
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BarrelSettings->TraceFX, ProjectileStartLocation, GetComponentRotation());
        if (TraceFXComponent)
            TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ProjectileEndLocation);
    }

    return bHit;
}

// TODO shoot via AnimNotify in weapon shoot montage
bool UBarrelComponent::ShootProjectile(const FVector& ViewLocation, const FVector& ViewDirection, AController* ShooterController)
{
#if ENABLE_DRAW_DEBUG
    bool bDrawDebugEnabled = GetDebugSubsystem()->IsDebugCategoryEnabled(DebugCategoryRangeWeapons);
#else
    bool bDrawDebugEnabled = false;
#endif

    CachedShooterController = ShooterController;
    const float ProjectileTraceDistance = 100000.f;
    AProjectile* CurrentProjectile = ProjectilesPool[CurrentProjectileIndex];
    CurrentProjectileIndex = (CurrentProjectileIndex + 1) % BarrelSettings->ProjectilePoolSize;
    
    FHitResult TraceResult;
    const FVector TraceEnd = ViewLocation + ViewDirection * ProjectileTraceDistance;

    FCollisionQueryParams CollisionQueryParams;
    CollisionQueryParams.AddIgnoredActor(GetOwner());
    CollisionQueryParams.AddIgnoredActor(GetDamagingActor());
    if (ShooterController)
        CollisionQueryParams.AddIgnoredActor(ShooterController->GetPawn());

    bool bHit = GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLocation, TraceEnd, ECC_Bullet, CollisionQueryParams);
    // DrawDebugLine(GetWorld(), ViewLocation, TraceEnd, FColor::Purple, false, 5, 0, 2);
    CurrentProjectile->SetActorLocation(GetComponentLocation());
    FVector ShootDirection = bHit || TraceResult.bBlockingHit
                                 ? (TraceResult.ImpactPoint - CurrentProjectile->GetActorLocation()).GetSafeNormal()
                                 : (TraceEnd - CurrentProjectile->GetActorLocation()).GetSafeNormal();

    if (bDrawDebugEnabled)
    {
        DrawDebugLine(GetWorld(), ViewLocation, TraceEnd, FColor::Orange, false, 5.f);
        DrawDebugLine(GetWorld(), GetComponentLocation(),
            GetComponentLocation() + ShootDirection * BarrelSettings->ProjectileSpeed, FColor::Blue, false, 5.f);
        if (bHit || TraceResult.bBlockingHit)
        DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, 10.f, 24, FColor::Orange, false, 2.f);
    }
    
    CurrentProjectile->SetActorRotation(ShootDirection.ToOrientationRotator());
    // : (TraceEnd - ViewLocation).GetSafeNormal();
  
    // CurrentProjectile->SetActorEnableCollision(true);
    if (!CurrentProjectile->ProjectileHitEvent.IsBoundToObject(this))
        CurrentProjectile->ProjectileHitEvent.BindUObject(this, &UBarrelComponent::OnProjectileHit);

    CurrentProjectile->LaunchProjectile(ShootDirection, GetOwner()->GetVelocity().Size() + BarrelSettings->ProjectileSpeed, ShooterController);
    return true;
}

void UBarrelComponent::OnProjectileHit(const FHitResult& HitResult, const FVector& Direction)
{
    if (IsValid(CachedShooterController))
        ApplyDamage(HitResult, Direction, CachedShooterController.Get());

    SpawnBulletHole(HitResult);
}

void UBarrelComponent::OnProjectileHit(AProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction)
{
    OnProjectileHit(HitResult, Direction);
    // Projectile->SetActorEnableCollision(false);
    Projectile->SetActorLocation(ProjectilesPoolLocation);
}

void UBarrelComponent::ApplyDamage(const FHitResult& ShotResult, const FVector& Direction, AController* ShooterController) const
{
    if (GetOwner()->GetLocalRole() < ROLE_AutonomousProxy)
        return;

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
        DamageEvent.Damage = Damage;
        
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
    {
        if (AudioActorOwner)
            AudioActorOwner->PlaySound(BarrelSettings->ShotSound);
        else
            UGameplayStatics::SpawnSoundAttached(BarrelSettings->ShotSound, GetAttachmentRoot());
    }

    // TODO tweak params
    UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetComponentLocation(), BarrelSettings->AiShotLoudness,
        GetOwner(), BarrelSettings->AiShotSoundRange, SoundStimulusTag_Shot);
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
