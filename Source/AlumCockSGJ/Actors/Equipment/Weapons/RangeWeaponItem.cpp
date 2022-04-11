#include "RangeWeaponItem.h"

#include "CommonConstants.h"
#include "Camera/CameraComponent.h"
#include "Characters/BaseHumanoidCharacter.h"
#include "Characters/Controllers/BasePlayerController.h"
#include "Components/AudioComponent.h"
#include "Components/Combat/WeaponBarrelComponent.h"
#include "Data/DataAssets/Items/RangeWeaponSettings.h"
#include "Data/DataAssets/Items/WeaponBarrelSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ARangeWeaponItem::ARangeWeaponItem()
{
    WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Weapon Mesh"));
    WeaponMeshComponent->SetupAttachment(RootComponent);

    PrimaryWeaponBarrelComponent = CreateDefaultSubobject<UWeaponBarrelComponent>(TEXT("PrimaryBarrel"));
    PrimaryWeaponBarrelComponent->SetupAttachment(WeaponMeshComponent, MuzzleSocketName);

    ScopeCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Scope"));
    ScopeCameraComponent->SetupAttachment(WeaponMeshComponent);

    AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
    AudioComponent->SetupAttachment(WeaponMeshComponent, MuzzleSocketName);
}

void ARangeWeaponItem::BeginPlay()
{
    Super::BeginPlay();
    checkf(IsValid(ItemSettings) && ItemSettings->IsA<URangeWeaponSettings>(),
        TEXT("Item settings must be a valid RangeWeaponSettings DataAsset"));
    RangeWeaponSettings = StaticCast<const URangeWeaponSettings*>(ItemSettings);
    TInlineComponentArray<UWeaponBarrelComponent*> BarrelComponents;
    GetComponents<UWeaponBarrelComponent>(BarrelComponents);
    for (auto BarrelComponent : BarrelComponents)
    {
        UWeaponBarrelComponent* Barrel = StaticCast<UWeaponBarrelComponent*>(BarrelComponent);
        Barrel->SetAmmo(Barrel->GetWeaponBarrelSettings()->ClipCapacity);
        Barrels.Add(Barrel);
    }

    ActiveBarrelIndex = 0;
    ActiveWeaponBarrel = Barrels[ActiveBarrelIndex];
    AmmoChangedEvent.ExecuteIfBound(ActiveWeaponBarrel->GetAmmo());
}

void ARangeWeaponItem::RegisterOnClient(UCharacterEquipmentComponent* EquipmentComponent)
{
    Super::RegisterOnClient(EquipmentComponent);
    if (GetLocalRole() == ROLE_AutonomousProxy)
    {
        AmmoChangedEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnAmmoChanged);
        OutOfAmmoEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnOutOfAmmo);
    }
}

bool ARangeWeaponItem::TryAddToEquipment(UCharacterEquipmentComponent* EquipmentComponent, const FPickUpItemData& PickUpItemData)
{
    auto DesignatedSlot = ItemSettings->DesignatedSlot;
    auto ExistingItem = EquipmentComponent->Loadout[(uint8)DesignatedSlot];
    if (IsValid(ExistingItem))
        EquipmentComponent->DropWeapon(ItemSettings->DesignatedSlot, PickUpItemData.PickUpLocation);
    
	bool bAdded = Super::TryAddToEquipment(EquipmentComponent, PickUpItemData);
	if (!bAdded)
		return false;

    if (PickUpItemData.bDropped)
        SetAmmo(PickUpItemData.InitialAmmo);

    EquipmentComponent->Loadout[(uint8)DesignatedSlot] = this;
    if (GetLocalRole() == ROLE_AutonomousProxy || StaticCast<ABaseCharacter*>(EquipmentComponent->GetOwner())->IsLocallyControlled())
    {
        AmmoChangedEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnAmmoChanged);
        OutOfAmmoEvent.BindUObject(EquipmentComponent, &UCharacterEquipmentComponent::OnOutOfAmmo);
    }
    
    if (EquipmentComponent->EquipmentSettings->bAutoEquipNewItem || ItemSettings->DesignatedSlot == EquipmentComponent->EquippedSlot)
        EquipmentComponent->EquipItem(ItemSettings->DesignatedSlot);
    
    return true;
}

void ARangeWeaponItem::OnEquipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
    Super::OnEquipped(CharacterEquipmentComponent);
    CharacterEquipmentComponent->EquippedMeleeWeapon = nullptr;
    CharacterEquipmentComponent->EquippedRangedWeapon = this;
    if (CanAim())
        CharacterEquipmentComponent->AimingSpeedChangedEvent.ExecuteIfBound(GetAimMaxSpeed());
    else
        CharacterEquipmentComponent->AimingSpeedResetEvent.ExecuteIfBound();

    CharacterEquipmentComponent->OnActiveWeaponBarrelChanged();
    AmmoChangedEvent.ExecuteIfBound(GetAmmo());
}

void ARangeWeaponItem::OnUnequipped(UCharacterEquipmentComponent* CharacterEquipmentComponent)
{
    Super::OnUnequipped(CharacterEquipmentComponent);
    CharacterEquipmentComponent->EquippedRangedWeapon = nullptr;
}

void ARangeWeaponItem::OnDropped(UCharacterEquipmentComponent* EquipmentComponent, APickableEquipmentItem* PickableEquipmentItem)
{
    PickableEquipmentItem->SetDroppedState(GetAmmo());
}

float ARangeWeaponItem::PlaySound(USoundCue* Sound)
{
    AudioComponent->SetSound(Sound);
    AudioComponent->Play();
    return AudioComponent->Sound->Duration;
}

void ARangeWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    FDoRepLifetimeParams ReplicationParams;
    ReplicationParams.Condition = ELifetimeCondition::COND_SimulatedOnly;
    ReplicationParams.RepNotifyCondition = ELifetimeRepNotifyCondition::REPNOTIFY_Always;
    DOREPLIFETIME_WITH_PARAMS(ARangeWeaponItem, ReplicatedShots, ReplicationParams)
}

EReticleType ARangeWeaponItem::GetReticleType() const
{
    return bAiming ? ActiveWeaponBarrel->GetWeaponBarrelSettings()->AimReticleType : RangeWeaponSettings->ReticleType;
}

#pragma region SHOOT

bool ARangeWeaponItem::TryStartFiring()
{
    // why not bFiring?
    bFiring = Shoot();
    return bFiring;
}

void ARangeWeaponItem::StopFiring()
{
    bFiring = false;
}

bool ARangeWeaponItem::Shoot()
{
    if (!bCanFire /* || GetWorld()->GetTimerManager().IsTimerActive(ShootTimer)*/)
        return false;
    
    int32 Ammo = GetAmmo();
    if (Ammo <= 0)
    {
        auto NoAmmoSFX = ActiveWeaponBarrel->GetWeaponBarrelSettings()->ShootWithNoAmmoSFX;
        if (IsValid(NoAmmoSFX))
        {
            AudioComponent->SetSound(NoAmmoSFX);
            AudioComponent->Play();
        }

        if (!bReloading)
        {
            OutOfAmmoEvent.ExecuteIfBound();
            StopFiring();
        }
        
        return false;
    }

    FVector ViewLocation;
    FRotator ViewRotation;
    auto ShooterController = CharacterOwner->GetController();
    ShooterController->GetPlayerViewPoint(ViewLocation, ViewRotation);
    FVector ViewDirection = ViewRotation.Vector();
    auto WeaponBarrelSettings = ActiveWeaponBarrel->GetWeaponBarrelSettings();

    TArray<FShotInfo> ShotsToFire;
    ShotsToFire.SetNumUninitialized(WeaponBarrelSettings->BulletsPerShot);
    for (auto i = 0; i < WeaponBarrelSettings->BulletsPerShot; i++)
    {
        // ActiveWeaponBarrel->Shoot(ViewLocation, ViewDirection + GetBulletSpreadOffset(ViewRotation), CachedShooterController);
        ShotsToFire[i] = FShotInfo(ViewLocation, ViewDirection + GetBulletSpreadOffset(ViewRotation));
    }

    ShootInternal(ShotsToFire);
    if (GetLocalRole() == ROLE_AutonomousProxy)
        Server_Shoot(ShotsToFire);
    else if (GetLocalRole() == ROLE_Authority)
        ReplicatedShots = ShotsToFire;
    
    return true;
}

void ARangeWeaponItem::ResetShot()
{
    bCanFire = true;
    if (!bFiring)
        return;
    
    switch (ActiveWeaponBarrel->GetWeaponBarrelSettings()->FireMode)
    {
    case EWeaponFireMode::Single:
        StopFiring();
        break;
    case EWeaponFireMode::FullAuto:
        {
            auto ShooterController = CharacterOwner->GetController();
            if (ShooterController && ShooterController->IsLocalController())
                Shoot();
        }
        break;
    default:
        StopFiring();
        break;
    }
}

FVector ARangeWeaponItem::GetBulletSpreadOffset(const FRotator& ShotOrientation) const
{
    float PitchAngle = FMath::RandRange(0.f, GetBulletSpreadAngleRad());
    if (FMath::IsNearlyZero(PitchAngle, KINDA_SMALL_NUMBER))
    {
        return FVector::ZeroVector;
    }

    float SpreadSize = FMath::Tan(PitchAngle);
    float RotationAngle = FMath::RandRange(0.f, 2 * PI);
    float SpreadY = FMath::Cos(RotationAngle);
    float SpreadZ = FMath::Sin(RotationAngle);

    return (ShotOrientation.RotateVector(FVector::UpVector) * SpreadZ + ShotOrientation.RotateVector(FVector::RightVector) * SpreadY) *
           SpreadSize;
}

float ARangeWeaponItem::GetBulletSpreadAngleRad() const
{
    const auto WeaponBarrelSettings = ActiveWeaponBarrel->GetWeaponBarrelSettings();
    return FMath::DegreesToRadians(bAiming ? WeaponBarrelSettings->AimSpreadAngle : WeaponBarrelSettings->SpreadAngle);
}

void ARangeWeaponItem::OnRep_Shots()
{
    ShootInternal(ReplicatedShots);
}

void ARangeWeaponItem::ShootInternal(const TArray<FShotInfo>& ShotInfos)
{
    if (!ActiveWeaponBarrel)
        return;

    auto CachedShooterController = CharacterOwner->GetController();
    for (const auto& ShotInfo : ShotInfos)
        ActiveWeaponBarrel->Shoot(ShotInfo.GetLocation(), ShotInfo.GetDirection(), CachedShooterController);

    int32 Ammo = GetAmmo();
    if (!ActiveWeaponBarrel->GetWeaponBarrelSettings()->bInfiniteClips)
        SetAmmo(Ammo - 1);

    auto WeaponBarrelSettings = ActiveWeaponBarrel->GetWeaponBarrelSettings();
    PlayAnimMontage(WeaponBarrelSettings->WeaponShootMontage);
    
    if (CachedShooterController && CachedShooterController->IsLocalController())
    {
        auto PlayerController = Cast<ABasePlayerController>(CachedShooterController);
        if (PlayerController)
            PlayerController->GetPlayerHUDWidget()->OnPlayerShoot(2.0);
    }

    bCanFire = false;
    GetWorld()->GetTimerManager().SetTimer(ShootTimer, this, &ARangeWeaponItem::ResetShot, GetShootTimerInterval(), false);
    
    ActiveWeaponBarrel->FinalizeShot();
    CharacterOwner->PlayAnimMontage(WeaponBarrelSettings->CharacterShootMontage);
}

void ARangeWeaponItem::Server_Shoot_Implementation(const TArray<FShotInfo>& Shots)
{
    ShootInternal(Shots);
}

bool ARangeWeaponItem::Server_Shoot_Validate(const TArray<FShotInfo>& Shots)
{
    // TODO also check for fire rate
    return GetAmmo() > 0; // && !GetWorld()->GetTimerManager().IsTimerActive(ShootTimer);
}

#pragma endregion SHOOT

#pragma region AIM

bool ARangeWeaponItem::StartAiming()
{
    if (CanAim() && !bAiming)
    {
        bAiming = true;
        return true;
    }

    return false;
}

void ARangeWeaponItem::StopAiming()
{
    bAiming = false;
}

#pragma endregion AIM

#pragma region RELOAD

void ARangeWeaponItem::StartReloading(float DesiredReloadDuration)
{
    bReloading = true;
    PlayAnimMontage(ActiveWeaponBarrel->GetWeaponBarrelSettings()->WeaponReloadMontage, DesiredReloadDuration);
}

void ARangeWeaponItem::StopReloading(bool bInterrupted)
{
    bReloading = false;
    if (bInterrupted)
    {
        WeaponMeshComponent->GetAnimInstance()->Montage_Stop(0.0f, ActiveWeaponBarrel->GetWeaponBarrelSettings()->WeaponReloadMontage);
    }
}

#pragma endregion RELOAD

#pragma region FIRE MODES

void ARangeWeaponItem::StartTogglingFireMode()
{
    bChangingFireMode = true;
}

void ARangeWeaponItem::CompleteTogglingFireMode()
{
    ActiveBarrelIndex = (ActiveBarrelIndex + 1) % Barrels.Num();
    ActiveWeaponBarrel = Barrels[ActiveBarrelIndex];
    AmmoChangedEvent.ExecuteIfBound(ActiveWeaponBarrel->GetAmmo());
    bChangingFireMode = false;
}

const UWeaponBarrelSettings* ARangeWeaponItem::GetNextWeaponBarrelSettings() const
{
    UWeaponBarrelComponent* NextBarrel = Barrels[(ActiveBarrelIndex + 1) % Barrels.Num()];
    return NextBarrel->GetWeaponBarrelSettings();
}

#pragma endregion FIRE MODES

void ARangeWeaponItem::SetAmmo(int32 NewAmmo)
{
    ActiveWeaponBarrel->SetAmmo(NewAmmo);
    AmmoChangedEvent.ExecuteIfBound(NewAmmo);
}

FTransform ARangeWeaponItem::GetForegripTransform() const
{
    return WeaponMeshComponent->GetSocketTransform(SocketForegrip);
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage, float DesiredDuration)
{
    if (!IsValid(AnimMontage))
        return -1.f;

    const float PlayRate = DesiredDuration > 0 ? AnimMontage->GetPlayLength() / DesiredDuration : 1;
    const auto AnimInstance = WeaponMeshComponent->GetAnimInstance();
    if (IsValid(AnimInstance))
    {
        return AnimInstance->Montage_Play(AnimMontage, PlayRate);
    }

    return -1.f;
}
